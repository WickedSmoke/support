/*
 * btree2.c (version 1.2.1)
 * Written and dedicated to the public domain in 2022 by Karl Robillard.
 *
 * Generate a static binary space partition for 2D, axis aligned boxes.
 */

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//#define BTREE2_REPORT
#define BTREE2_BISECT_CENTER

#ifndef BTREE2_DIM
#define BTREE2_DIM  int16_t
#endif
#ifndef BTREE2_DATA
#define BTREE2_DATA int
#endif
#ifndef BTREE2_LEAF_SIZE
#define BTREE2_LEAF_SIZE    6
#endif
#ifndef BTREE2_EDGE_EPSILON
#define BTREE2_EDGE_EPSILON 2
#endif

typedef BTREE2_DIM  bt2dim_t;
typedef BTREE2_DATA bt2data_t;

// BTree2Split flags
#define BTREE2_AXIS_X   1
#define BTREE2_L_LEAF   2
#define BTREE2_H_LEAF   4

typedef struct {
    uint16_t flags;
    uint8_t  countL;
    uint8_t  countH;
    uint16_t indexL;    // Index into split or leaves array.
    uint16_t indexH;    // Index into split or leaves array.
    bt2dim_t splitPos;
}
BTree2Split;

typedef struct {
    bt2dim_t  x, y;
}
BTree2Point;

typedef struct {
    bt2dim_t  x, y;     // Minimum
    bt2dim_t  x2, y2;   // Maxiumum (x + width, y + height)
}
BTree2Bound;

typedef struct {
    bt2dim_t  x, y;     // Minimum
    bt2dim_t  x2, y2;   // Maxiumum (x + width, y + height)
    bt2data_t data;
}
BTree2Box;

typedef struct {
    uint16_t splitCount;
    uint16_t leavesSize;
    BTree2Split split;      // split[splitCount]
    // uint16_t leaves[leavesSize];
    // BTree2Box boxes[boxCount];
}
BTree2;

#define BTREE2_SPLIT(bt)    &bt->split
#define BTREE2_LEAVES(bt)   ((uint16_t*) (&bt->split + bt->splitCount))
#define BTREE2_BYTES(bt) \
    (4 + sizeof(BTree2Split)*bt->splitCount + sizeof(uint16_t)*bt->leavesSize)

typedef struct {
    const BTree2Box* inbox;
    BTree2Point* center;        // Center point for each inbox.
    uint16_t* leaves;           // Box index array for each leaf.
    BTree2Split* split;
    int leavesSize;
    int splitCount;
#ifdef BTREE2_REPORT
    int depth;
#endif
}
BTree2Gen;


#ifndef BTREE2_PICK_ONLY
#define ALLOC(T,N)  (T*) malloc(sizeof(T) * N)

#ifdef BTREE2_BISECT_CENTER
static void btree2_centersBound(BTree2Bound* bnd, const BTree2Point* center,
                                const uint16_t* list, int listSize)
{
    bt2dim_t p;
    const BTree2Point* cpoint = center + list[0];
    bnd->x = bnd->x2 = cpoint->x;
    bnd->y = bnd->y2 = cpoint->y;

    for (int i = 1; i < listSize; ++i) {
        cpoint = center + list[i];

        p = cpoint->x;
        if (p < bnd->x)
            bnd->x = p;
        else if (p > bnd->x2)
            bnd->x2 = p;

        p = cpoint->y;
        if (p < bnd->y)
            bnd->y = p;
        else if (p > bnd->y2)
            bnd->y2 = p;
    }
}
#endif

bt2dim_t btree2_splitEdge(const BTree2Box* inbox, const uint16_t* list, int listSize,
                          int xyOff, bt2dim_t boundL, bt2dim_t boundH)
{
    bt2dim_t curEdge;
    bt2dim_t mid = (boundL + boundH) / 2;
    bt2dim_t edge = mid;
    int32_t edgeDist = 0x7fffffff;
    int32_t d;

    for (int i = 0; i < listSize; ++i) {
        const BTree2Box* box = inbox + list[i];
        const bt2dim_t* v0 = &box->x + xyOff;
        curEdge = v0[0] - 1;    // box x or y
        if (curEdge > boundL) {
            d = mid - curEdge;
            if (d < 0)
                d = -d;
            if (d < edgeDist) {
                edgeDist = d;
                edge = curEdge;
                if (d <= BTREE2_EDGE_EPSILON)
                    break;
            }
        }

        curEdge = v0[2];    // box x2 or y2
        if (curEdge < boundH) {
            d = mid - curEdge;
            if (d < 0)
                d = -d;
            if (d < edgeDist) {
                edgeDist = d;
                edge = curEdge;
                if (d <= BTREE2_EDGE_EPSILON)
                    break;
            }
        }
    }
    return edge;
}

static int btree2_intersects(const BTree2Bound* a, const BTree2Box* b)
{
    return (a->x < b->x2 && a->x2 > b->x &&
            a->y < b->y2 && a->y2 > b->y);
}

static int btree2_partition(BTree2Gen* gen, const BTree2Bound* bound,
                            const uint16_t* list, int listSize)
{
    BTree2Bound subL, subH;
    BTree2Split sp;
    int dx, dy;
    int si = gen->splitCount++;

    subL = *bound;
    subH = *bound;

#ifdef BTREE2_BISECT_CENTER
    // Select bisect axis based on the distribution of box centers.
    {
    BTree2Bound cbound;
    btree2_centersBound(&cbound, gen->center, list, listSize);
    dx = cbound.x2 - cbound.x;
    dy = cbound.y2 - cbound.y;
    }
#else
    // Using a simple bisect partitioning scheme based on the enclosing
    // boundary dimensions.

    dx = bound->x2 - bound->x;
    dy = bound->y2 - bound->y;
#endif

#ifdef BTREE2_REPORT
    printf("BT2 partition %d (%d,%d %d,%d) %c\n",
            gen->depth, bound->x, bound->y, bound->x2, bound->y2,
            dx > dy ? 'X':'Y');
    ++gen->depth;
#endif

    if (dx > dy) {
        sp.flags  = BTREE2_AXIS_X;
        sp.splitPos = btree2_splitEdge(gen->inbox, list, listSize, 0, bound->x, bound->x2);

        subL.x2 = sp.splitPos;
        subH.x  = sp.splitPos;
    } else {
        sp.flags  = 0;
        sp.splitPos = btree2_splitEdge(gen->inbox, list, listSize, 1, bound->y, bound->y2);

        subL.y2 = sp.splitPos;
        subH.y  = sp.splitPos;
    }

    sp.countL = sp.countH = 0;
    sp.indexL = sp.indexH = 0;

    {
    int part, i, inCount;
    BTree2Bound* sub;
    uint16_t* inside = ALLOC(uint16_t, listSize);
    uint16_t* inp;

    for (part = 0; part < 2; ++part) {
        sub = part ? &subH : &subL;
        for (inp = inside, i = 0; i < listSize; ++i) {
            if (btree2_intersects(sub, gen->inbox + list[i]))
                *inp++ = list[i];
        }

        inCount = inp - inside;
        if (inCount <= BTREE2_LEAF_SIZE) {
            // Leaf node reached.
            int li = gen->leavesSize;

            if (part) {
                sp.flags |= BTREE2_H_LEAF;
                sp.countH = inCount;
                sp.indexH = li;
            } else {
                sp.flags |= BTREE2_L_LEAF;
                sp.countL = inCount;
                sp.indexL = li;
            }

            for (i = 0; i < inCount; ++i)
                gen->leaves[li++] = inside[i];

            gen->leavesSize = li;
        } else {
            // Sub-partition.
            int index = btree2_partition(gen, sub, inside, inCount);
            if (part)
                sp.indexH = index;
            else
                sp.indexL = index;
        }
    }

    free(inside);
    }

#ifdef BTREE2_REPORT
    --gen->depth;
#endif

    gen->split[si] = sp;
    return si;
}

/*
 * Return BTree2 pointer which caller must free().
 */
BTree2* btree2_generate(BTree2Gen* gen, const BTree2Box* inbox, int boxCount)
{
    BTree2Bound bound;
    BTree2* hdr;
    const BTree2Box* box = inbox;
    size_t splitMax, leavesMax, bufTotal;
    int i;

    // Calculate bounding box.
    bound = *((const BTree2Bound*) box);
    for (i = 1; i < boxCount; ++i) {
        ++box;
        if (bound.x > box->x)
            bound.x = box->x;
        if (bound.y > box->y)
            bound.y = box->y;
        if (bound.x2 < box->x2)
            bound.x2 = box->x2;
        if (bound.y2 < box->y2)
            bound.y2 = box->y2;
    }

    splitMax  = boxCount;
    leavesMax = boxCount * 4;
    bufTotal = sizeof(BTree2Split) * splitMax +
               sizeof(BTree2Point) * boxCount +
               sizeof(uint16_t) * leavesMax +
               sizeof(uint16_t) * boxCount;

    // Pointers assigned in order of struct size to maintain natural alignment.
    gen->split  = (BTree2Split*) malloc(bufTotal);
    gen->center = (BTree2Point*) (gen->split + splitMax);
    gen->leaves = (uint16_t*) (gen->center + boxCount);

    gen->inbox = inbox;
    gen->leavesSize = 0;
    gen->splitCount = 0;
#ifdef BTREE2_REPORT
    gen->depth = 0;
#endif

    {
    uint16_t* inside = gen->leaves + leavesMax;
#ifdef BTREE2_BISECT_CENTER
    BTree2Point* cpoint = gen->center;
    box = inbox;
    for (i = 0; i < boxCount; ++i) {
        inside[i] = i;

        cpoint->x = (box->x + box->x2) / 2;
        cpoint->y = (box->y + box->y2) / 2;
        ++cpoint;
        ++box;
    }
#else
    for (i = 0; i < boxCount; ++i)
        inside[i] = i;
#endif
    btree2_partition(gen, &bound, inside, boxCount);
    }

#ifdef BTREE2_REPORT
    printf("BT2 boxCount: %d leavesSize: %d splitCount: %d\n\n",
           boxCount, gen->leavesSize, gen->splitCount);
#endif
    assert(gen->leavesSize <= (int) leavesMax);
    assert(gen->splitCount <= (int) splitMax);

    // Transfer generator buffers to a minimally sized, single chunk of memory.
    {
    size_t sizeSpl = sizeof(BTree2Split) * gen->splitCount;
    size_t sizeLvs = sizeof(uint16_t)    * gen->leavesSize;
    hdr = (BTree2*) malloc(4 + sizeSpl + sizeLvs);
    hdr->splitCount = gen->splitCount;
    hdr->leavesSize = gen->leavesSize;
    memcpy(BTREE2_SPLIT(hdr), gen->split, sizeSpl);
    memcpy(BTREE2_LEAVES(hdr), gen->leaves, sizeLvs);
    }

    free(gen->split);   // Free all working buffers.
    return hdr;
}
#endif

const BTree2Box* btree2_pick(const BTree2* tree, const BTree2Box* boxes,
                             bt2dim_t x, bt2dim_t y)
{
    const BTree2Split* split = BTREE2_SPLIT(tree);
    const BTree2Split* sp = split;
    int part, leafIndex, bc;

    while (1) {
        part = 0;
        if (sp->flags & BTREE2_AXIS_X) {
            if (x > sp->splitPos)
                part = 1;
        } else {
            if (y > sp->splitPos)
                part = 1;
        }

        if (part) {
            if (sp->flags & BTREE2_H_LEAF) {
                bc = sp->countH;
                leafIndex = sp->indexH;
                break;
            }
            sp = split + sp->indexH;
        } else {
            if (sp->flags & BTREE2_L_LEAF) {
                bc = sp->countL;
                leafIndex = sp->indexL;
                break;
            }
            sp = split + sp->indexL;
        }
    }

    {
    const uint16_t* li  = BTREE2_LEAVES(tree) + leafIndex;
    const uint16_t* end = li + bc;
    for (; li != end; ++li) {
        const BTree2Box* box = boxes + *li;
        if (x >= box->x && x < box->x2 &&
            y >= box->y && y < box->y2)
            return box;
    }
    }
    return NULL;
}
