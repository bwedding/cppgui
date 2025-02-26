import React, { useState, useCallback } from 'react';
import { Menu, Settings, HelpCircle, Download, Users, Minus, Plus, Square, X } from 'lucide-react';
import MenuBar from './MenuBar.tsx';
import MenuButton from '@/components/ui/menubutton.tsx';
import ChildWindowManager from './ChildWindowManager.tsx';
import FileDialogComponent from '../ui/FileDialogComponent.tsx';

declare global {
  interface Window {
    webView?: {
      ExecuteScriptAsync(script: string): Promise<void>;
    };
  }
}

const TitleBar: React.FC = () => {
  const [menuOpen, setMenuOpen] = useState(false);
  const [profileOpen, setProfileOpen] = useState(false);
  const [zoom, setZoom] = useState(100);
  const [isDragging, setIsDragging] = useState(false);
  const [ORViewWindow, setORViewWindow] = useState<Window | null>(null);
  const [stopwatchTime, setStopwatchTime] = useState(0);

  // Handle window dragging
  const handleMouseDown = useCallback((e: React.MouseEvent<HTMLElement>) => {
    // Check if we clicked a button or something with role="button"
    const target = e.target as HTMLElement;
    if (
      target.closest('button') || 
      target.closest('[role="button"]') || 
      target.closest('[role="menuitem"]') || 
      target.closest('[data-radix-collection-item]')
    ) {
      return;
    }
    setIsDragging(true);
    window.chrome?.webview?.hostObjects?.sync?.native?.StartWindowDrag();
  }, []);

  const handleMouseUp = useCallback(() => {
    if (isDragging) {
      setIsDragging(false);
    }
  }, [isDragging]);

  // Window control handlers
  const handleMinimize = () => {
    window.chrome?.webview?.hostObjects?.sync?.native?.MinimizeWindow();
  };

  const handleMaximize = () => {
    window.chrome?.webview?.hostObjects?.sync?.native?.MaximizeWindow();
  };

  const handleQuit = () => {
    if (ORViewWindow && !ORViewWindow.closed) {
      console.log('Closing child window');
      ORViewWindow.close();
    }
    window.chrome?.webview?.hostObjects?.sync?.native?.CloseWindow();
  };

  // Attach global mouse up handler
  React.useEffect(() => {
    document.addEventListener('mouseup', handleMouseUp);
    return () => document.removeEventListener('mouseup', handleMouseUp);
  }, [handleMouseUp]);

  const user = {
    name: "Bruce Wedding",
    email: "Bruce.Wedding@realheart.se",
    initials: "BW"
  };

  const openORView = useCallback(() => {
    const features = 'width=1920,height=1080,menubar=no,toolbar=no,location=no,status=no';
    const localFilePath = 'D:/source/WebView2Samples/AnimalMonitor/App/Frontend/UI/dist-subset/src/subset.html';
    const newWindow = window.open(localFilePath, 'ORView', features);
    if (newWindow) {
      setORViewWindow(newWindow);
      localStorage.setItem('log', 'Child window opened');
      newWindow.addEventListener('load', async () => {
        const childWebView = (newWindow as Window).webView;
        if (childWebView) {
          await childWebView.ExecuteScriptAsync("document.documentElement.requestFullscreen()");
        }
      });
      // Add event listener to close child window when parent closes
    }
  }, []);

  return (
    <>
      <ChildWindowManager 
        childWindow={ORViewWindow}
        setChildWindow={setORViewWindow}
        stopwatchTime={stopwatchTime}
        setStopwatchTime={setStopwatchTime}
      />
      <div 
        className="w-full bg-neutral-800 text-gray-100 select-none"
        onMouseDown={handleMouseDown}
        style={{ cursor: isDragging ? 'move' : 'default' }}
      >
        <div className="flex items-center justify-between ml-4 px-1 py-0 mt-0 mb-2 border-b border-neutral-600">
          {/* Left section */}
          <div className="w-36 h-14 flex items-center justify-center">
            <img 
              src="logo.png" 
              alt="Logo" 
              className="w-full h-full object-contain"
            />
          </div>
           {/* Center section */}
          <div className="flex-1 text-center">
            <h1 className="text-lg text-neutral-500 font-semibold">Animal Trial Monitor v1.0</h1>
          </div>

          {/* Right section */}
          <div className="flex items-center space-x-4">
            <MenuBar 
              setORViewWindow={setORViewWindow} 
              menuOpen={menuOpen} 
              setMenuOpen={setMenuOpen} 
              profileOpen={profileOpen} 
              setProfileOpen={setProfileOpen} 
              zoom={zoom} 
              setZoom={setZoom} 
              isDragging={isDragging} 
              setIsDragging={setIsDragging} 
              handleMinimize={handleMinimize} 
              handleMaximize={handleMaximize} 
              handleQuit={handleQuit} 
            />
            <button
              onClick={() => setProfileOpen(!profileOpen)}
              className="w-8 h-8 bg-red-600 rounded-full flex items-center justify-center cursor-pointer"
            >
              <span className="text-sm font-medium">{user.initials}</span>
            </button>

            {/* Window controls */}
            <div className="flex items-center">
              <button
                onClick={handleMinimize}
                className="pl-0 pr-1 hover:bg-neutral-900 text-white hover:text-neutral-100"
              >
                <Minus size={18} />
              </button>
              <button
                onClick={handleMaximize}
                className="pl-0 pr-1 hover:bg-neutral-900 white hover:text-neutral-100"
              >
                <Square size={18} />
              </button>
              <button
                onClick={handleQuit}
                className="pl-0 pr-2 hover:bg-neutral-900 white hover:text-neutral-100"
              >
                <X size={18} />
              </button>
            </div>
          </div>
        </div>
        {/* Profile dropdown */}
        {profileOpen && (
          <div className="absolute right-4 top-12 w-64 bg-neutral-800 rounded-lg shadow-lg p-2">
            <div className="px-4 py-2">
              <div className="font-medium">{user.name}</div>
              <div className="text-sm text-neutral-400">{user.email}</div>
            </div>
            <div className="border-t border-neutral-700" />
            <button className="w-full px-4 py-2 text-left hover:bg-neutral-700 rounded mt-1 cursor-pointer">
              Sign out
            </button>
          </div>
        )}
      </div>
    </>
  );
};

export default TitleBar;