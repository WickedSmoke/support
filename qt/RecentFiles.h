#ifndef RECENTFILES_H
#define RECENTFILES_H
/*
    RecentFiles v1.1 - Show recently used files on a QMenu.

    Written and dedicated to the public domain by Karl Robillard.
*/


#include <QString>


class QAction;
class QMenu;

class RecentFiles
{
public:

    void install( QMenu* menu, QObject* receiver, const char* method );
    void setFiles( const QStringList& );
    void addFile( const QString* );
    QString fileOpened( QObject* sender );

    QStringList files;

private:

    enum { MaxRecentFiles = 5 };
    QAction* _action[ MaxRecentFiles ];
    QAction* _separator;
};


#endif  // RECENTFILES_H
