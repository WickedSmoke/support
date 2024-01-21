/*
 * USB HID Usage Ids for keyboards (Page 7)
 * https://www.usb.org/hid
 */

enum HIDUsageKey {
	HKEY_Reserved,
	HKEY_ErrorRollOver,
	HKEY_POSTFail,
	HKEY_ErrorUndefined,
	HKEY_A,			// 0x04
	HKEY_B,
	HKEY_C,
	HKEY_D,
	HKEY_E,
	HKEY_F,
	HKEY_G,
	HKEY_H,
	HKEY_I,
	HKEY_J,
	HKEY_K,
	HKEY_L,
	HKEY_M,
	HKEY_N,
	HKEY_O,
	HKEY_P,
	HKEY_Q,
	HKEY_R,
	HKEY_S,
	HKEY_T,
	HKEY_U,
	HKEY_V,
	HKEY_W,
	HKEY_X,
	HKEY_Y,
	HKEY_Z,			// 0x1d
	HKEY_1,			// !, 0x1e
	HKEY_2,			// @
	HKEY_3,			// #
	HKEY_4,			// $
	HKEY_5,			// %
	HKEY_6,			// ^
	HKEY_7,			// &
	HKEY_8,			// *
	HKEY_9,			// (
	HKEY_0,			// ), 0x27
	HKEY_Return,	// 0x28
	HKEY_Escape,
	HKEY_Backspace,
	HKEY_Tab,
	HKEY_Space,
	HKEY_Minus,		// -_
	HKEY_Equal,		// =+
	HKEY_LBrace,	// [{
	HKEY_RBrace,	// }]
	HKEY_Backslash,	// \| (US keyboard only)
	HKEY_Hash,		// #~ (Non-US)
	HKEY_Semicolon,	// ;:
	HKEY_Quote,		// '"
	HKEY_Grave,		// `~
	HKEY_Comma,		// ,<
	HKEY_Period,	// .>
	HKEY_Slash,		// /?
	HKEY_CapsLock,	// 0x39
	HKEY_F1,		// 0x3a
	HKEY_F2,
	HKEY_F3,
	HKEY_F4,
	HKEY_F5,
	HKEY_F6,
	HKEY_F7,
	HKEY_F8,
	HKEY_F9,
	HKEY_F10,
	HKEY_F11,
	HKEY_F12,			// 0x45
	HKEY_PrintScreen,	// 0x46
	HKEY_ScrollLock,
	HKEY_Pause,
	HKEY_Insert,
	HKEY_Home,
	HKEY_PageUp,
	HKEY_Delete,		// (Forward Delete)
	HKEY_End,
	HKEY_PageDown,
	HKEY_RightArrow,
	HKEY_LeftArrow,
	HKEY_DownArrow,
	HKEY_UpArrow,		// 0x52
	HKEY_KP_NumLock,	// 0x53
	HKEY_KP_Divide,		// /
	HKEY_KP_Multiply,	// *
	HKEY_KP_Subtract,	// -
	HKEY_KP_Add,		// +
	HKEY_KP_Enter,
	HKEY_KP_1,		// End
	HKEY_KP_2,		// Down
	HKEY_KP_3,		// PageDown
	HKEY_KP_4,		// LeftArrow
	HKEY_KP_5,
	HKEY_KP_6,		// RightArrow
	HKEY_KP_7,		// Home
	HKEY_KP_8,		// UpArrow
	HKEY_KP_9,		// PageUp
	HKEY_KP_0,		// Insert
	HKEY_KP_Delete,	// ., 0x63
	HKEY_LS,		// <>, \| (Non-US, near LeftShift key)
	HKEY_AppMenu,	// (next to RWin key)
	HKEY_Power,
	HKEY_KP_Equal,	// 0x67
	HKEY_F13,		// 0x68
	HKEY_F14,
	HKEY_F15,
	HKEY_F16,
	HKEY_F17,
	HKEY_F18,
	HKEY_F19,
	HKEY_F20,
	HKEY_F21,
	HKEY_F22,
	HKEY_F23,
	HKEY_F24,		// 0x73
	HKEY_Execute,	// 0x74
	HKEY_Help,
	HKEY_Menu,
	HKEY_Select,
	HKEY_Stop,
	HKEY_Again,		// Redo
	HKEY_Undo,
	HKEY_Cut,
	HKEY_Copy,
	HKEY_Paste,
	HKEY_Find,
	HKEY_Mute,		// 0x7f
	HKEY_VolumeUp,
	HKEY_VolumeDown,
	HKEY_LockingCapsLock,
	HKEY_LockingNumLock,
	HKEY_LockingScrollLock,	// 0x84

	// (International & keypad keys omitted here)

	HKEY_LControl = 0xe0,
	HKEY_LShift,
	HKEY_LAlt,		// Option
	HKEY_LCmd,		// LeftGUI/Super/Win/Cmd
	HKEY_RControl,
	HKEY_RShift,
	HKEY_RAlt,		// Option
	HKEY_RCmd		// RightGUI/Super/Win/Cmd, 0xe7
};
