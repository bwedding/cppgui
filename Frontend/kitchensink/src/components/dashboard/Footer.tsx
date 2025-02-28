import React, { useState, memo, useCallback } from 'react';
import { StatusBarClock } from './Clock';
import { AlertTriangle } from "lucide-react";
import { Button } from "@/components/ui/button";

// Memoized error message component
const ErrorMessage = memo(({ time, text, onClear }: { 
  time: string, 
  text: string, 
  onClear: () => void 
}) => (
  <>
    <div className="flex items-center text-red-400 text-sm">
      <AlertTriangle className="w-4 h-4 mr-2" />
      <span className="mr-1">{time}</span>
      <span className="mr-3">{text}</span>
    </div>
    <Button
      variant="secondary"
      size="sm"
      className="h-6 px-3 text-xs bg-neutral-700 hover:bg-neutral-600"
      onClick={onClear}
    >
      Acknowledge
    </Button>
  </>
));

ErrorMessage.displayName = 'ErrorMessage';

// Memoized no alerts component
const NoAlerts = memo(() => (
  <div className="text-sm text-neutral-400">No active alerts</div>
));

NoAlerts.displayName = 'NoAlerts';

const Footer = memo(() => {
  const [errorMessage, setErrorMessage] = useState({
    time: "15:02",
    text: "Lost contact with right half of the heart",
  });

  const clearError = useCallback(() => setErrorMessage(null), []);

  return (
    <div className="mb-4 w-full h-8 bg-neutral-800 border-t border-neutral-700 shadow-[inset_0_2px_4px_rgba(0,0,0,0.3)] flex items-center">
      {/* Left segment - Error message and acknowledge button */}
      <div className="flex items-center px-4 border-r border-neutral-700">
        {errorMessage ? (
          <ErrorMessage 
            time={errorMessage.time} 
            text={errorMessage.text} 
            onClear={clearError} 
          />
        ) : (
          <NoAlerts />
        )}
      </div>

      {/* Middle segment - Empty */}
      <div className="flex-1 border-r border-neutral-700"></div>

      {/* Right segment - Clock and stopwatch */}
      <div className="px-4 border-l border-neutral-700">
        <StatusBarClock />
      </div>
    </div>
  );
});

Footer.displayName = 'Footer';

export default Footer;
