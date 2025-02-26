import { useState, useCallback, useEffect } from "react";
import { useAtom } from 'jotai';
import { isRunningAtom, elapsedTimeAtom } from './stopwatchAtoms.ts';
import {
  AlertDialog,
  AlertDialogAction,
  AlertDialogCancel,
  AlertDialogContent,
  AlertDialogDescription,
  AlertDialogFooter,
  AlertDialogHeader,
  AlertDialogPortal,
  AlertDialogTitle,
  AlertDialogTrigger,
} from "@/components/ui/alert-dialog.tsx";
import {
  MessageCircle,
  MonitorDot,
  MoreHorizontal,
  LineChart,
  Info,
  Power,
  Activity,
  Timer,
  StopCircle,
  RotateCcw,
  Settings,
} from "lucide-react";
import {
  DropdownMenu,
  DropdownMenuContent,
  DropdownMenuItem,
  DropdownMenuLabel,
  DropdownMenuSeparator,
  DropdownMenuTrigger,
} from "@/components/ui/dropdown-menu.tsx";
import { Button } from "@/components/ui/button.tsx";
import { Card } from "@/components/ui/card.tsx";
import { Switch } from "@/components/ui/switch.tsx";
import { ScrollArea } from "@/components/ui/scroll-area.tsx";
import {
  AlertTriangle,
  X,
} from "lucide-react";
import { pressureCardVisibilityAtom } from "@/lib/datastore";
import {
  Dialog,
  DialogContent,
  DialogHeader,
  DialogTitle,
} from "@/components/ui/dialog";
import { Checkbox } from "@/components/ui/checkbox";
import { Label } from "@/components/ui/label";
import { atom } from 'jotai';

interface Message {
  time: string;
  text: string;
  type: "error" | "warning" | "info";
  read?: boolean;
}

export const messagesAtom = atom<Message[]>([
  {
    time: "17:10",
    text: "Right target atrium pressure set to 8.00 mmHg",
    type: "info",
    read: false
  },
  {
    time: "17:10",
    text: "Left target atrium pressure set to 6.00 mmHg",
    type: "info",
    read: false
  },
  {
    time: "15:07",
    text: "Regained contact with right half of the heart",
    type: "info",
    read: false
  },
  {
    time: "15:06",
    text: "MCU Left: Initializing",
    type: "warning",
    read: false
  },
  {
    time: "15:06",
    text: "MCU Left: Initializing",
    type: "warning",
    read: false
  },
  {
    time: "15:02",
    text: "Lost contact with right half of the heart",
    type: "error",
    read: false
  },
  {
    time: "15:06",
    text: "Right target atrium pressure set to 8.00 mmHg",
    type: "info",
    read: false
  },
]);

interface MenuBarProps {
  setORViewWindow: (window: Window | null) => void;
  menuOpen: boolean;
  setMenuOpen: (open: boolean) => void;
  profileOpen: boolean;
  setProfileOpen: (open: boolean) => void;
  zoom: number;
  setZoom: (zoom: number) => void;
  isDragging: boolean;
  setIsDragging: (dragging: boolean) => void;
  handleMinimize: () => void;
  handleMaximize: () => void;
  handleQuit: () => void;
}

const MessageList = ({ onClose }: { onClose: () => void }) => {
  const [messages, setMessages] = useAtom(messagesAtom);
  
  useEffect(() => {
    setMessages(prev => prev.map(msg => ({ ...msg, read: true })));
  }, []);

  return (
    <Card className="absolute top-12 right-0 w-[400px] bg-neutral-900 border-gray-800 z-50">
      <div className="flex items-center justify-between p-4 border-b border-gray-800">
        <h2 className="text-lg font-semibold text-white">MESSAGES</h2>
        <Button
          variant="ghost"
          size="icon"
          onClick={onClose}
          className="text-gray-400 hover:text-gray-100"
        >
          <X className="h-4 w-4" />
        </Button>
      </div>
      <ScrollArea className="h-[400px]">
        <div className="flex flex-col">
          <div className="p-4 border-b border-gray-800">
            <div className="text-sm font-medium text-gray-400 mb-2">Today</div>
            {messages.map((message, index) => (
              <div key={index} className="flex items-start gap-3 mb-4 last:mb-0">
                <div className="text-sm text-gray-400 w-12">{message.time}</div>
                <div className="flex-1 flex items-start gap-2">
                  {message.type === "error" && (
                    <AlertTriangle className="h-4 w-4 text-red-500 mt-0.5 flex-shrink-0" />
                  )}
                  {message.type === "warning" && (
                    <AlertTriangle className="h-4 w-4 text-orange-500 mt-0.5 flex-shrink-0" />
                  )}
                  {message.type === "info" && <div className="w-4" />}
                  <span
                    className={`text-sm ${
                      message.type === "error" 
                        ? "text-red-500" 
                        : message.read 
                          ? "text-gray-300" 
                          : "text-white font-medium"
                    }`}
                  >
                    {message.text}
                  </span>
                </div>
              </div>
            ))}
          </div>
          <div className="p-4">
            <div className="text-sm font-medium text-gray-400 mb-2">
              Yesterday
            </div>
            <div className="flex items-start gap-3 mb-4">
              <div className="text-sm text-gray-400 w-12">22:06</div>
              <span className="text-sm text-gray-300">
                Right target atrium pressure set to 8.00 mmHg
              </span>
            </div>
          </div>
        </div>
      </ScrollArea>
    </Card>
  );
};

const MenuBar: React.FC<MenuBarProps> = ({
  setORViewWindow,
  menuOpen,
  setMenuOpen,
  profileOpen,
  setProfileOpen,
  zoom,
  setZoom,
  isDragging,
  setIsDragging,
  handleMinimize,
  handleMaximize,
  handleQuit,
}) => {
  const [showMessages, setShowMessages] = useState(false);
  const [messages] = useAtom(messagesAtom);
  const [useMedicalSensors, setUseMedicalSensors] = useState(false);
  const [isRunning, setIsRunning] = useAtom(isRunningAtom);
  const [elapsedTime, setElapsedTime] = useAtom(elapsedTimeAtom);
  const [showConfigureORView, setShowConfigureORView] = useState(false);
  const [pressureCardVisibility, setPressureCardVisibility] = useAtom(pressureCardVisibilityAtom);
  const unreadCount = messages.filter(msg => !msg.read).length;

  const openORView = useCallback(() => {
    console.log('MenuBar: Opening OR View window');
    const features = 'width=1920,height=1080,menubar=no,toolbar=no,location=no,status=no';
    //const localFilePath = 'D:/source/WebView2Samples/AnimalMonitor/App/Frontend/UI/dist-subset/src/subset.html';
    const localFilePath = 'C:/Users/bruce/source/Win32ReactUI/AnimalMonitor/App/Frontend/UI/dist-subset/src/subset.html';
    
    const newWindow = window.open(localFilePath, 'ORView', features);
    if (newWindow) {
      console.log('MenuBar: Window opened successfully');
      setORViewWindow(newWindow);
    }
  }, [setORViewWindow]);

  const handleStartStop = () => {
    setIsRunning(prev => !prev);
    if (isRunning) {
      console.log('Timer paused - elapsed time is:', elapsedTime);
    } else {
      console.log('Timer resumed from:', elapsedTime);
    }
  };

  const handleReset = () => {
    setIsRunning(false);
    setElapsedTime(0);
  };

  const handleVisibilityChange = (metricId: string, checked: boolean) => {
    setPressureCardVisibility(prev => ({
      ...prev,
      [metricId]: checked
    }));
  };

  return (
    <div className="relative flex items-center justify-end gap-4 py-0">
      <div className="relative">
        <Button
          variant="ghost"
          size="icon"
          onClick={() => setShowMessages(!showMessages)}
          className="text-white 900 hover:bg-neutral-900 text-white hover:text-gray-100"
        >
          <MessageCircle className="h-5 w-5" />
          {unreadCount > 0 && (
            <div className="absolute -top-1 -right-1 bg-red-500 text-white text-xs rounded-full w-4 h-4 flex items-center justify-center">
              {unreadCount}
            </div>
          )}
        </Button>
        {showMessages && <MessageList onClose={() => setShowMessages(false)} />}
      </div>
      <DropdownMenu>
        <DropdownMenuTrigger asChild>
          <Button
            variant="ghost"
            size="icon"
            className="text-white 900 hover:bg-neutral-900 text-white hover:text-gray-100"
          >
            <MoreHorizontal className="h-5 w-5" />
          </Button>
        </DropdownMenuTrigger>
        <DropdownMenuContent className="w-56 bg-neutral-800 text-neutral-100 border-neutral-700">
          <DropdownMenuLabel className="text-neutral-400">
            MORE TOOLS
          </DropdownMenuLabel>
          <DropdownMenuItem className="focus:bg-neutral-600 focus:text-white">
            <LineChart className="mr-2 h-4 w-4" />
            Create graph...
          </DropdownMenuItem>
          <DropdownMenuItem className="focus:bg-neutral-600 focus:text-white">
            <Info className="mr-2 h-4 w-4" />
            About RealHeart
          </DropdownMenuItem>
          <DropdownMenuSeparator className="bg-neutral-700" />
          <DropdownMenuLabel className="text-neutral-400">
            DEVELOPERS ONLY
          </DropdownMenuLabel>
          <DropdownMenuItem 
            className="focus:bg-neutral-600 focus:text-white"
            onSelect={(e) => {
              handleStartStop();
            }}
          >
            <div className="flex items-center">
              {isRunning ? (
                <StopCircle className="mr-2 h-4 w-4" />
              ) : (
                <Timer className="mr-2 h-4 w-4" />
              )}
              {isRunning ? 'Pause Timer' : 'Resume Timer'}
            </div>
          </DropdownMenuItem>
          <AlertDialog>
            <AlertDialogTrigger asChild>
              <DropdownMenuItem 
                className="focus:bg-neutral-600 focus:text-white"
                onSelect={(e) => {
                  e.preventDefault();
                  console.log('Reset menu clicked');
                }}
              >
                <RotateCcw className="mr-2 h-4 w-4" />
                Reset Timer
              </DropdownMenuItem>
            </AlertDialogTrigger>
            <AlertDialogPortal>
              <AlertDialogContent className="bg-neutral-800 border-neutral-700">
                <AlertDialogHeader>
                  <AlertDialogTitle className="text-white">Reset Timer?</AlertDialogTitle>
                  <AlertDialogDescription className="text-neutral-400">
                    Are you sure you want to reset the timer? This action cannot be undone.
                  </AlertDialogDescription>
                </AlertDialogHeader>
                <AlertDialogFooter>
                  <AlertDialogCancel className="bg-neutral-700 text-white hover:bg-neutral-600">Cancel</AlertDialogCancel>
                  <AlertDialogAction 
                    className="bg-red-600 text-white hover:bg-red-700"
                    onClick={() => {
                      console.log('Reset confirmed');
                      handleReset();
                    }}
                  >
                    Reset
                  </AlertDialogAction>
                </AlertDialogFooter>
              </AlertDialogContent>
            </AlertDialogPortal>
          </AlertDialog>
          <DropdownMenuItem
            className="focus:bg-neutral-600 focus:text-white"
            onClick={() => openORView()}
          >
            <MonitorDot className="mr-2 h-4 w-4" />
            Operating Room View
          </DropdownMenuItem>
          <DropdownMenuItem
            className="focus:bg-neutral-600 focus:text-white"
            onClick={() => setShowConfigureORView(true)}
          >
            <Settings className="mr-2 h-4 w-4" />
            Configure OR View
          </DropdownMenuItem>
          <DropdownMenuItem className="focus:bg-neutral-600 focus:text-white">
            <Power className="mr-2 h-4 w-4" />
            Force reset
          </DropdownMenuItem>
          <DropdownMenuItem className="text-red-500 focus:bg-red-900 focus:text-red-200">
            <Power className="mr-2 h-4 w-4" />
            Stop TAH
          </DropdownMenuItem>
          <DropdownMenuItem className="focus:bg-neutral-600 focus:text-white flex items-center justify-between">
            <div className="flex items-center">
              <Activity className="mr-2 h-4 w-4" />
              Use medical sensors
            </div>
            <Switch
              checked={useMedicalSensors}
              onCheckedChange={(e) => {
                setUseMedicalSensors(!useMedicalSensors);
              }}
              className="ml-2"
            />
          </DropdownMenuItem>
        </DropdownMenuContent>
      </DropdownMenu>

      <Dialog open={showConfigureORView} onOpenChange={setShowConfigureORView}>
        <DialogContent className="bg-neutral-800 text-white border-neutral-700 w-[20vw] max-w-[20vw]">
          <DialogHeader>
            <DialogTitle className="text-white text-xl font-bold text-center">Configure OR View</DialogTitle>
          </DialogHeader>
          <div className="grid gap-4 py-4">
            {Object.entries(pressureCardVisibility).map(([metricId, isVisible]) => (
              <div key={metricId} className="flex items-center space-x-2">
                <Checkbox
                  id={metricId}
                  checked={isVisible}
                  onCheckedChange={(checked) => handleVisibilityChange(metricId, checked as boolean)}
                  className="border-neutral-400 text-lg text-white data-[state=checked]:bg-blue-500 data-[state=checked]:border-blue-500"
                />
                <Label htmlFor={metricId} className="text-white">{metricId} Pressure Card</Label>
              </div>
            ))}
          </div>
          <div className="mt-4 flex justify-end">
            <button
              onClick={() => setShowConfigureORView(false)}
              className="px-4 py-2 bg-blue-500 text-white rounded hover:bg-blue-600 transition-colors"
            >
              OK
            </button>
          </div>
        </DialogContent>
      </Dialog>
    </div>
  );
};

export default MenuBar;
