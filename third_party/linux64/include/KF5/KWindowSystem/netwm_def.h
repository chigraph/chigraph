/*

  Copyright (c) 2000 Troll Tech AS
  Copyright (c) 2003 Lubos Lunak <l.lunak@kde.org>

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

*/

#ifndef   netwm_def_h
#define   netwm_def_h
#include <kwindowsystem_export.h>
#include <QFlags>

/**
  Simple point class for NET classes.

  This class is a convenience class defining a point x, y.  The existence of
  this class is to keep the implementation from being dependent on a
  separate framework/library.

  NETPoint is only used by the NET API. Usually QPoint is the
  appropriate class for representing a point.

  @author Bradley T. Hughes <bhughes@trolltech.com>
**/

struct NETPoint {
    /**
       Constructor to initialize this point to 0,0.
    **/
    NETPoint() : x(0), y(0) { }

    /*
       Public data member.
    **/
    int x, ///< x coordinate.
        y;   ///< y coordinate
};

/**
  Simple size class for NET classes.

  This class is a convenience class defining a size width by height.  The
  existence of this class is to keep the implementation from being dependent
  on a separate framework/library.

  NETSize is only used by the NET API. Usually QSize is the
  appropriate class for representing a size.

  @author Bradley T. Hughes <bhughes@trolltech.com>
**/

struct NETSize {
    /**
       Constructor to initialize this size to 0x0
    **/
    NETSize() : width(0), height(0) { }

    /*
       Public data member.
    **/
    int width,  ///< Width.
        height;   ///< Height.
};

/**
   Simple rectangle class for NET classes.

   This class is a convenience class defining a rectangle as a point x,y with a
   size width by height.  The existence of this class is to keep the implementation
   from being dependent on a separate framework/library;

   NETRect is only used by the NET API. Usually QRect is the
   appropriate class for representing a rectangle.
**/
struct NETRect {
    /**
       Position of the rectangle.

       @see NETPoint
    **/
    NETPoint pos;

    /**
       Size of the rectangle.

       @see NETSize
    **/
    NETSize size;
};

/**
   Simple icon class for NET classes.

   This class is a convenience class defining an icon of size width by height.
   The existence of this class is to keep the implementation from being
   dependent on a separate framework/library.

   NETIcon is only used by the NET API. Usually QIcon is the
   appropriate class for representing an icon.
**/

struct NETIcon {
    /**
       Constructor to initialize this icon to 0x0 with data=0
    **/
    NETIcon() : data(0) { }

    /**
       Size of the icon.

       @see NETSize
    **/
    NETSize size;

    /**
       Image data for the icon.  This is an array of 32bit packed CARDINAL ARGB
       with high byte being A, low byte being B. First two bytes are width, height.
       Data is in rows, left to right and top to bottom.
    **/
    unsigned char *data;
};

/**
   Partial strut class for NET classes.

   This class is a convenience class defining a strut with left, right, top and
   bottom border values, and ranges for them.  The existence of this class is to
   keep the implementation from being dependent on a separate framework/library.
   See the _NET_WM_STRUT_PARTIAL property in the NETWM spec.
**/

struct NETExtendedStrut {
    /**
       Constructor to initialize this struct to 0,0,0,0
    **/
    NETExtendedStrut() : left_width(0), left_start(0), left_end(0),
        right_width(0), right_start(0), right_end(0), top_width(0), top_start(0), top_end(0),
        bottom_width(0), bottom_start(0), bottom_end(0) {}

    /**
       Left border of the strut, width and range.
           **/
    int left_width, left_start, left_end;

    /**
       Right border of the strut, width and range.
    **/
    int right_width, right_start, right_end;

    /**
       Top border of the strut, width and range.
           **/
    int top_width, top_start, top_end;

    /**
       Bottom border of the strut, width and range.
           **/
    int bottom_width, bottom_start, bottom_end;

};

/**
   @deprecated use NETExtendedStrut

   Simple strut class for NET classes.

   This class is a convenience class defining a strut with left, right, top and
   bottom border values.  The existence of this class is to keep the implementation
   from being dependent on a separate framework/library. See the _NET_WM_STRUT
   property in the NETWM spec.
**/

struct NETStrut {
    /**
       Constructor to initialize this struct to 0,0,0,0
    **/
    NETStrut() : left(0), right(0), top(0), bottom(0) { }

    /**
       Left border of the strut.
           **/
    int left;

    /**
       Right border of the strut.
    **/
    int right;

    /**
       Top border of the strut.
           **/
    int top;

    /**
       Bottom border of the strut.
           **/
    int bottom;
};

/**
   Simple multiple monitor topology class for NET classes.

   This class is a convenience class, defining a multiple monitor topology
   for fullscreen applications that wish to be present on more than one
   monitor/head. As per the _NET_WM_FULLSCREEN_MONITORS hint in the EWMH spec,
   this topology consists of 4 monitor indices such that the bounding rectangle
   is defined by the top edge of the top monitor, the bottom edge of the bottom
   monitor, the left edge of the left monitor, and the right edge of the right
   monitor. See the _NET_WM_FULLSCREEN_MONITORS hint in the EWMH spec.
**/

struct NETFullscreenMonitors {
    /**
       Constructor to initialize this struct to -1,0,0,0 (an initialized,
       albeit invalid, topology).
    **/
    NETFullscreenMonitors() : top(-1), bottom(0), left(0), right(0) { }

    /**
       Monitor index whose top border defines the top edge of the topology.
    **/
    int top;

    /**
       Monitor index whose bottom border defines the bottom edge of the topology.
    **/
    int bottom;

    /**
       Monitor index whose left border defines the left edge of the topology.
    **/
    int left;

    /**
       Monitor index whose right border defines the right edge of the topology.
    **/
    int right;

    /**
       Convenience check to make sure that we are not holding the initial (invalid)
       values. Note that we don't want to call this isValid() because we're not
       actually validating the monitor topology here, but merely that our initial
       values were overwritten at some point by real (non-negative) monitor indices.
    **/
    bool isSet() const
    {
        return (top != -1);
    }
};

/**
  Base namespace class.

  The NET API is an implementation of the NET Window Manager Specification.

  This class is the base class for the NETRootInfo and NETWinInfo classes, which
  are used to retrieve and modify the properties of windows. To keep
  the namespace relatively clean, all enums are defined here.

  @see http://www.freedesktop.org/standards/wm-spec/
 **/

class KWINDOWSYSTEM_EXPORT NET
{
public:
    /**
       Application role.  This is used internally to determine how several action
       should be performed (if at all).
    **/

    enum Role {
        /**
           indicates that the application is a client application.
        **/
        Client,
        /**
           indicates that the application is a window manager application.
        **/
        WindowManager
    };

    /**
       Window type.
    **/

    enum WindowType {
        /**
           indicates that the window did not define a window type.
        **/
        Unknown  = -1,
        /**
           indicates that this is a normal, top-level window
        **/
        Normal   = 0,
        /**
           indicates a desktop feature. This can include a single window
           containing desktop icons with the same dimensions as the screen, allowing
           the desktop environment to have full control of the desktop, without the
           need for proxying root window clicks.
        **/
        Desktop  = 1,
        /**
           indicates a dock or panel feature
        **/
        Dock     = 2,
        /**
           indicates a toolbar window
        **/
        Toolbar  = 3,
        /**
           indicates a pinnable (torn-off) menu window
        **/
        Menu     = 4,
        /**
           indicates that this is a dialog window
        **/
        Dialog   = 5,
        /**
               @deprecated has unclear meaning and is KDE-only
        **/
        Override = 6, // NON STANDARD
        /**
           indicates a toplevel menu (AKA macmenu). This is a KDE extension to the
           _NET_WM_WINDOW_TYPE mechanism.
        **/
        TopMenu  = 7, // NON STANDARD
        /**
           indicates a utility window
        **/
        Utility  = 8,
        /**
           indicates that this window is a splash screen window.
        **/
        Splash   = 9,
        /**
           indicates a dropdown menu (from a menubar typically)
        **/
        DropdownMenu = 10,
        /**
           indicates a popup menu (a context menu typically)
        **/
        PopupMenu = 11,
        /**
           indicates a tooltip window
        **/
        Tooltip = 12,
        /**
           indicates a notification window
        **/
        Notification = 13,
        /**
           indicates that the window is a list for a combobox
        **/
        ComboBox = 14,
        /**
           indicates a window that represents the dragged object during DND operation
        **/
        DNDIcon = 15,
        /**
            indicates an On Screen Display window (such as volume feedback)
            @since 5.6
        **/
        OnScreenDisplay = 16 // NON STANDARD
    };

    /**
        Values for WindowType when they should be OR'ed together, e.g.
        for the properties argument of the NETRootInfo constructor.
    **/
    enum WindowTypeMask {
        NormalMask   = 1u << 0, ///< @see Normal
        DesktopMask  = 1u << 1, ///< @see Desktop
        DockMask     = 1u << 2, ///< @see Dock
        ToolbarMask  = 1u << 3, ///< @see Toolbar
        MenuMask     = 1u << 4, ///< @see Menu
        DialogMask   = 1u << 5, ///< @see Dialog
        OverrideMask = 1u << 6, ///< @see Override
        TopMenuMask  = 1u << 7, ///< @see TopMenu
        UtilityMask  = 1u << 8, ///< @see Utility
        SplashMask   = 1u << 9, ///< @see Splash
        DropdownMenuMask = 1u << 10, ///< @see DropdownMenu
        PopupMenuMask    = 1u << 11, ///< @see PopupMenu
        TooltipMask      = 1u << 12, ///< @see Tooltip
        NotificationMask = 1u << 13, ///< @see Notification
        ComboBoxMask     = 1u << 14, ///< @see ComboBox
        DNDIconMask      = 1u << 15, ///< @see DNDIcon
        OnScreenDisplayMask = 1u << 16, ///< NON STANDARD @see OnScreenDisplay @since 5.6
        AllTypesMask     = 0U - 1 ///< All window types.
    };
    Q_DECLARE_FLAGS(WindowTypes, WindowTypeMask)

    /**
     * Returns true if the given window type matches the mask given
     * using WindowTypeMask flags.
     */
    static bool typeMatchesMask(WindowType type, WindowTypes mask);

    /**
       Window state.

       To set the state of a window, you'll typically do something like:
       \code
         KWindowSystem::setState( winId(), NET::SkipTaskbar | NET::SkipPager );
       \endcode

       for example to not show the window on the taskbar and desktop pager.
       winId() is a function of QWidget()

       Note that KeepAbove (StaysOnTop) and KeepBelow are meant as user preference and
       applications should avoid setting these states themselves.
    **/

    enum State {
        /**
           indicates that this is a modal dialog box. The WM_TRANSIENT_FOR hint
           MUST be set to indicate which window the dialog is a modal for, or set to
           the root window if the dialog is a modal for its window group.
        **/
        Modal        = 1u << 0,
        /**
           indicates that the Window Manager SHOULD keep the window's position
           fixed on the screen, even when the virtual desktop scrolls. Note that this is
           different from being kept on all desktops.
        **/
        Sticky       = 1u << 1,
        /**
           indicates that the window is vertically maximized.
        **/
        MaxVert      = 1u << 2,
        /**
           indicates that the window is horizontally maximized.
        **/
        MaxHoriz     = 1u << 3,
        /**
           convenience value. Equal to MaxVert | MaxHoriz.
        **/
        Max = MaxVert | MaxHoriz,
        /**
           indicates that the window is shaded (rolled-up).
        **/
        Shaded       = 1u << 4,
        /**
           indicates that a window should not be included on a taskbar.
        **/
        SkipTaskbar  = 1u << 5,
        /**
           indicates that a window should on top of most windows (but below fullscreen
           windows).
        **/
        KeepAbove    = 1u << 6,
        /**
           @deprecated This is an obsolete name for KeepAbove.
        **/
        StaysOnTop   = KeepAbove,   // NOT STANDARD
        /**
           indicates that a window should not be included on a pager.
        **/
        SkipPager    = 1u << 7,
        /**
           indicates that a window should not be visible on the screen (e.g. when minimised).
           Only the window manager is allowed to change it.
        **/
        Hidden       = 1u << 8,
        /**
           indicates that a window should fill the entire screen and have no window
           decorations.
        **/
        FullScreen   = 1u << 9,
        /**
           indicates that a window should be below most windows (but above any desktop windows).
        **/
        KeepBelow    = 1u << 10,
        /**
           there was an attempt to activate this window, but the window manager prevented
           this. E.g. taskbar should mark such window specially to bring user's attention to
           this window. Only the window manager is allowed to change it.
        **/
        DemandsAttention = 1u << 11
    };
    Q_DECLARE_FLAGS(States, State)

    /**
       Direction for WMMoveResize.

       When a client wants the Window Manager to start a WMMoveResize, it should
       specify one of:

       @li TopLeft
       @li Top
       @li TopRight
       @li Right
       @li BottomRight
       @li Bottom
       @li BottomLeft
       @li Left
       @li Move (for movement only)
       @li KeyboardSize (resizing via keyboard)
       @li KeyboardMove (movement via keyboard)
    **/

    enum Direction {
        TopLeft      = 0,
        Top          = 1,
        TopRight     = 2,
        Right        = 3,
        BottomRight  = 4,
        Bottom       = 5,
        BottomLeft   = 6,
        Left         = 7,
        Move         = 8,  // movement only
        KeyboardSize = 9,  // size via keyboard
        KeyboardMove = 10, // move via keyboard
        MoveResizeCancel = 11 // to ask the WM to stop moving a window
    };

    /**
       Client window mapping state.  The class automatically watches the mapping
       state of the client windows, and uses the mapping state to determine how
       to set/change different properties. Note that this is very lowlevel
       and you most probably don't want to use this state.
    **/
    enum MappingState {
        /**
           indicates the client window is visible to the user.
        **/
        Visible = 1, //NormalState,
        /**
           indicates that neither the client window nor its icon is visible.
        **/
        Withdrawn = 0, //WithdrawnState,
        /**
           indicates that the client window is not visible, but its icon is.
           This can be when the window is minimized or when it's on a
           different virtual desktop. See also NET::Hidden.
        **/
        Iconic = 3 // IconicState
    };

    /**
      Actions that can be done with a window (_NET_WM_ALLOWED_ACTIONS).
    **/
    enum Action {
        ActionMove           = 1u << 0,
        ActionResize         = 1u << 1,
        ActionMinimize       = 1u << 2,
        ActionShade          = 1u << 3,
        ActionStick          = 1u << 4,
        ActionMaxVert        = 1u << 5,
        ActionMaxHoriz       = 1u << 6,
        ActionMax            = ActionMaxVert | ActionMaxHoriz,
        ActionFullScreen     = 1u << 7,
        ActionChangeDesktop  = 1u << 8,
        ActionClose          = 1u << 9
    };
    Q_DECLARE_FLAGS(Actions, Action)

    /**
       Supported properties.  Clients and Window Managers must define which
       properties/protocols it wants to support.

       Root/Desktop window properties and protocols:

       @li Supported
       @li ClientList
       @li ClientListStacking
       @li NumberOfDesktops
       @li DesktopGeometry
       @li DesktopViewport
       @li CurrentDesktop
       @li DesktopNames
       @li ActiveWindow
       @li WorkArea
       @li SupportingWMCheck
       @li VirtualRoots
       @li CloseWindow
       @li WMMoveResize

       Client window properties and protocols:

       @li WMName
       @li WMVisibleName
       @li WMDesktop
       @li WMWindowType
       @li WMState
       @li WMStrut  (obsoleted by WM2ExtendedStrut)
       @li WMGeometry
       @li WMFrameExtents
       @li WMIconGeometry
       @li WMIcon
       @li WMIconName
       @li WMVisibleIconName
       @li WMHandledIcons
       @li WMPid
       @li WMPing

       ICCCM properties (provided for convenience):

       @li XAWMState

    **/

    enum Property {
        // root
        Supported             = 1u << 0,
        ClientList            = 1u << 1,
        ClientListStacking    = 1u << 2,
        NumberOfDesktops      = 1u << 3,
        DesktopGeometry       = 1u << 4,
        DesktopViewport       = 1u << 5,
        CurrentDesktop        = 1u << 6,
        DesktopNames          = 1u << 7,
        ActiveWindow          = 1u << 8,
        WorkArea              = 1u << 9,
        SupportingWMCheck     = 1u << 10,
        VirtualRoots          = 1u << 11,
        //
        CloseWindow           = 1u << 13,
        WMMoveResize          = 1u << 14,

        // window
        WMName                = 1u << 15,
        WMVisibleName         = 1u << 16,
        WMDesktop             = 1u << 17,
        WMWindowType          = 1u << 18,
        WMState               = 1u << 19,
        WMStrut               = 1u << 20,
        WMIconGeometry        = 1u << 21,
        WMIcon                = 1u << 22,
        WMPid                 = 1u << 23,
        WMHandledIcons        = 1u << 24,
        WMPing                = 1u << 25,
        XAWMState             = 1u << 27,
        WMFrameExtents        = 1u << 28,

        // Need to be reordered
        WMIconName            = 1u << 29,
        WMVisibleIconName     = 1u << 30,
        WMGeometry            = 1u << 31,
        WMAllProperties       = ~0u
    };
    Q_DECLARE_FLAGS(Properties, Property)

    /**
        Supported properties. This enum is an extension to NET::Property,
        because them enum is limited only to 32 bits.

        Client window properties and protocols:

        @li WM2UserTime
        @li WM2StartupId
        @li WM2TransientFor mainwindow for the window (WM_TRANSIENT_FOR)
        @li WM2GroupLeader  group leader (window_group in WM_HINTS)
        @li WM2AllowedActions
        @li WM2RestackWindow
        @li WM2MoveResizeWindow
        @li WM2ExtendedStrut
        @li WM2TemporaryRules internal, for kstart
        @li WM2WindowClass  WM_CLASS
        @li WM2WindowRole   WM_WINDOW_ROLE
        @li WM2ClientMachine WM_CLIENT_MACHINE
        @li WM2ShowingDesktop
        @li WM2Opacity _NET_WM_WINDOW_OPACITY
        @li WM2DesktopLayout _NET_DESKTOP_LAYOUT
        @li WM2FullPlacement _NET_WM_FULL_PLACEMENT
        @li WM2FullscreenMonitors _NET_WM_FULLSCREEN_MONITORS
        @li WM2Urgency urgency hint in WM_HINTS (see ICCCM 4.1.2.4)
        @li WM2Input input hint (input in WM_HINTS, see ICCCM 4.1.2.4)
        @li WM2Protocols see NET::Protocol
        @li WM2InitialMappingState initial state hint of WM_HINTS (see ICCCM 4.1.2.4)
        @li WM2IconPixmap icon pixmap and mask in WM_HINTS (see ICCCM 4.1.2.4)
        @li WM2OpaqueRegion
        @li WM2DesktopFileName the base name of the desktop file name or the full path to the desktop file
    **/
    enum Property2 {
        WM2UserTime            = 1u << 0,
        WM2StartupId           = 1u << 1,
        WM2TransientFor        = 1u << 2,
        WM2GroupLeader         = 1u << 3,
        WM2AllowedActions      = 1u << 4,
        WM2RestackWindow       = 1u << 5,
        WM2MoveResizeWindow    = 1u << 6,
        WM2ExtendedStrut       = 1u << 7,
        WM2KDETemporaryRules   = 1u << 8, // NOT STANDARD
        WM2WindowClass         = 1u << 9,
        WM2WindowRole          = 1u << 10,
        WM2ClientMachine       = 1u << 11,
        WM2ShowingDesktop      = 1u << 12,
        WM2Opacity             = 1u << 13,
        WM2DesktopLayout       = 1u << 14,
        WM2FullPlacement       = 1u << 15,
        WM2FullscreenMonitors  = 1u << 16,
        WM2FrameOverlap        = 1u << 17, // NOT STANDARD
        WM2Activities          = 1u << 18, // NOT STANDARD @since 4.6
        WM2BlockCompositing    = 1u << 19, // NOT STANDARD @since 4.7, STANDARD @since 5.17
        WM2KDEShadow           = 1u << 20, // NOT Standard @since 4.7
        WM2Urgency             = 1u << 21, // @since 5.3
        WM2Input               = 1u << 22, // @since 5.3
        WM2Protocols           = 1u << 23, // @since 5.3
        WM2InitialMappingState = 1u << 24, // @since 5.5
        WM2IconPixmap          = 1u << 25, // @since 5.7
        WM2OpaqueRegion        = 1u << 25, // @since 5.7
        WM2DesktopFileName     = 1u << 26, // NOT STANDARD @since 5.28
        WM2AllProperties       = ~0u
    };
    Q_DECLARE_FLAGS(Properties2, Property2)

    /**
       Sentinel value to indicate that the client wishes to be visible on
       all desktops.
     **/
    enum { OnAllDesktops = -1 };

    /**
       Source of the request.
    **/
    // must match the values for data.l[0] field in _NET_ACTIVE_WINDOW message
    enum RequestSource {
        /**
          @internal indicates that the source of the request is unknown
        **/
        FromUnknown = 0, // internal
        /**
           indicates that the request comes from a normal application
        **/
        FromApplication = 1,
        /**
           indicated that the request comes from pager or similar tool
        **/
        FromTool = 2
    };

    /**
      Orientation.
    **/
    enum Orientation {
        OrientationHorizontal = 0,
        OrientationVertical = 1
    };

    /**
     Starting corner for desktop layout.
    **/
    enum DesktopLayoutCorner {
        DesktopLayoutCornerTopLeft = 0,
        DesktopLayoutCornerTopRight = 1,
        DesktopLayoutCornerBottomLeft = 2,
        DesktopLayoutCornerBottomRight = 3
    };

    /**
     * Protocols supported by the client.
     * See ICCCM 4.1.2.7.
     *
     * @since 5.3
     **/
    enum Protocol {
        NoProtocol = 0,
        TakeFocusProtocol = 1 << 0, ///< WM_TAKE_FOCUS
        DeleteWindowProtocol = 1 << 1, ///< WM_DELETE_WINDOW
        PingProtocol = 1 << 2, ///< _NET_WM_PING from EWMH
        SyncRequestProtocol = 1 << 3, ///< _NET_WM_SYNC_REQUEST from EWMH
        ContextHelpProtocol = 1 << 4 ///< _NET_WM_CONTEXT_HELP, NON STANDARD!
    };
    Q_DECLARE_FLAGS(Protocols, Protocol)

    /**
     Compares two X timestamps, taking into account wrapping and 64bit architectures.
     Return value is like with strcmp(), 0 for equal, -1 for time1 < time2, 1 for time1 > time2.
    */
    static int timestampCompare(unsigned long time1, unsigned long time2);
    /**
     Returns a difference of two X timestamps, time2 - time1, where time2 must be later than time1,
     as returned by timestampCompare().
    */
    static int timestampDiff(unsigned long time1, unsigned long time2);

};

Q_DECLARE_OPERATORS_FOR_FLAGS(NET::Properties)
Q_DECLARE_OPERATORS_FOR_FLAGS(NET::Properties2)
Q_DECLARE_OPERATORS_FOR_FLAGS(NET::WindowTypes)
Q_DECLARE_OPERATORS_FOR_FLAGS(NET::States)
Q_DECLARE_OPERATORS_FOR_FLAGS(NET::Actions)
Q_DECLARE_OPERATORS_FOR_FLAGS(NET::Protocols)

#endif // netwm_def_h
