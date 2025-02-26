import { useState, useEffect, useMemo, memo } from 'react';
import { Card } from '@/components/ui/card.tsx';
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
  AlertDialogTitle,
  AlertDialogTrigger,
} from "@/components/ui/alert-dialog";
import { TooltipProvider, Tooltip, TooltipTrigger, TooltipContent } from "@/components/ui/tooltip";

export {
  AlertDialog,
  AlertDialogAction,
  AlertDialogCancel,
  AlertDialogContent,
  AlertDialogDescription,
  AlertDialogFooter,
  AlertDialogHeader,
  AlertDialogTitle,
  AlertDialogTrigger,
};

// Static date formatting options
const DATE_OPTIONS: Intl.DateTimeFormatOptions = { 
  weekday: 'long', 
  year: 'numeric', 
  month: 'long', 
  day: 'numeric' 
};

const TIME_OPTIONS: Intl.DateTimeFormatOptions = {
  hour12: false,
  hour: '2-digit',
  minute: '2-digit',
  second: '2-digit'
};

const STATUS_BAR_DATE_OPTIONS: Intl.DateTimeFormatOptions = {
  year: 'numeric',
  month: 'short',
  day: 'numeric'
};

// Helper functions
const formatStopwatch = (totalSeconds: number): string => {
  const days = Math.floor(totalSeconds / (24 * 60 * 60));
  const hours = Math.floor((totalSeconds % (24 * 60 * 60)) / (60 * 60));
  const minutes = Math.floor((totalSeconds % (60 * 60)) / 60);
  const seconds = Math.floor(totalSeconds % 60);
  
  if (days > 0) {
    return `${days}:${hours.toString().padStart(2, "0")}:${minutes.toString().padStart(2, "0")}:${seconds.toString().padStart(2, "0")}`;
  }
  
  if (hours > 0) {
    return `${hours}:${minutes.toString().padStart(2, "0")}:${seconds.toString().padStart(2, "0")}`;
  }
  
  return `${minutes}:${seconds.toString().padStart(2, "0")}`;
};

const formatStatusBarTime = (time: Date): string => {
  const timeStr = time.toLocaleTimeString('en-US', TIME_OPTIONS);
  const dateStr = time.toLocaleDateString('en-US', STATUS_BAR_DATE_OPTIONS);
  return `${dateStr} ${timeStr}`;
};

const formatStatusBarStopwatch = (seconds: number): string => {
  const minutes = Math.floor(seconds / 60);
  const hours = Math.floor(minutes / 60);
  return `${hours.toString().padStart(2, '0')}:${(minutes % 60).toString().padStart(2, '0')}:${(seconds % 60).toString().padStart(2, '0')}`;
};

// Memoized components for better performance
const TimeDisplay = memo(({ hours, minutes, seconds }: { hours: string, minutes: string, seconds: string }) => (
  <div className="text-5xl font-bold mb-2">
    {hours}:{minutes}:{seconds}
  </div>
));
TimeDisplay.displayName = 'TimeDisplay';

const DateDisplay = memo(({ date }: { date: string }) => (
  <div className="text-xl text-neutral-400 mb-4">
    {date}
  </div>
));
DateDisplay.displayName = 'DateDisplay';

const StopwatchDisplay = memo(({ stopwatch }: { stopwatch: string }) => (
  <>
    <div className="text-3xl text-neutral-400 mb-2">
      Trial Run Time
    </div>
    <div className="text-5xl font-mono">
      {stopwatch}
    </div>
  </>
));
StopwatchDisplay.displayName = 'StopwatchDisplay';

const ClockStopwatch = memo(() => {
  // Clock state
  const [time, setTime] = useState(new Date());
  const [isRunning] = useAtom(isRunningAtom);
  const [elapsedTime, setElapsedTime] = useAtom(elapsedTimeAtom);

  // Clock effect - update every second
  useEffect(() => {
    const timer = setInterval(() => setTime(new Date()), 1000);
    return () => clearInterval(timer);
  }, []);

  // Stopwatch effect - update when running
  useEffect(() => {
    if (!isRunning) return;
    
    const timer = setInterval(() => {
      setElapsedTime(prev => prev + 1);
    }, 1000);
    
    return () => clearInterval(timer);
  }, [isRunning, setElapsedTime]);

  // Memoize formatted values
  const formattedValues = useMemo(() => {
    const hours = time.getHours().toString().padStart(2, "0");
    const minutes = time.getMinutes().toString().padStart(2, "0");
    const seconds = time.getSeconds().toString().padStart(2, "0");
    const date = time.toLocaleDateString('en-US', DATE_OPTIONS);
    const stopwatch = formatStopwatch(elapsedTime);

    return { hours, minutes, seconds, date, stopwatch };
  }, [time, elapsedTime]);

  return (
    <div className="flex flex-col items-center">
      <Card className="w-fit p-4 bg-neutral-900 text-white">
        <div className="flex flex-col items-center">
          <TimeDisplay 
            hours={formattedValues.hours} 
            minutes={formattedValues.minutes} 
            seconds={formattedValues.seconds} 
          />
          <DateDisplay date={formattedValues.date} />
          <div className="h-px w-full bg-neutral-700 mb-4"></div>
          <StopwatchDisplay stopwatch={formattedValues.stopwatch} />
        </div>
      </Card>
    </div>
  );
});

ClockStopwatch.displayName = 'ClockStopwatch';

export const StatusBarClock = () => {
  const [time, setTime] = useState(new Date());
  const [isRunning] = useAtom(isRunningAtom);
  const [elapsedTime, setElapsedTime] = useAtom(elapsedTimeAtom);

  // Clock effect - update every second
  useEffect(() => {
    const timer = setInterval(() => setTime(new Date()), 1000);
    return () => clearInterval(timer);
  }, []);

  // Stopwatch effect - update when running
  useEffect(() => {
    if (!isRunning) return;
    
    const timer = setInterval(() => {
      setElapsedTime(prev => prev + 1);
    }, 1000);
    
    return () => clearInterval(timer);
  }, [isRunning, setElapsedTime]);

  // Memoize formatted values
  const formattedValues = useMemo(() => ({
    time: formatStatusBarTime(time),
    stopwatch: formatStatusBarStopwatch(elapsedTime)
  }), [time, elapsedTime]);

  return (
    <TooltipProvider>
      <Tooltip>
        <TooltipTrigger>
          <div className="flex items-center text-sm text-neutral-400">
            <span>{formattedValues.time}</span>
            {isRunning && (
              <>
                <div className="border-l border-neutral-700 mx-4 h-4" />
                <span className="text-green-400">⏱{formattedValues.stopwatch}</span>
              </>
            )}
          </div>
        </TooltipTrigger>
        <TooltipContent>
          <p>System Time & Stopwatch</p>
        </TooltipContent>
      </Tooltip>
    </TooltipProvider>
  );
};

StatusBarClock.displayName = 'StatusBarClock';

export default ClockStopwatch;