import React from "react";
import { Card } from "../ui/card.tsx";
import {
  Tooltip,
  TooltipContent,
  TooltipProvider,
  TooltipTrigger,
} from "../ui/tooltip.tsx";
import {
  Activity,
  Battery,
  Signal,
  Wifi,
  AlertTriangle,
  Power,
  Server,
} from "lucide-react";

interface StatusIndicatorProps {
  icon: React.ElementType;
  label: string;
  status: "normal" | "warning" | "error";
  tooltipText: string;
}

const StatusIndicator = ({
  icon: Icon,
  label,
  status = "normal",
  tooltipText,
}: StatusIndicatorProps) => {
  const statusColors = {
    normal: "text-green-500",
    warning: "text-orange-500",
    error: "text-red-500",
  };

  return (
    <TooltipProvider>
      <Tooltip>
        <TooltipTrigger>
          <div className={`flex flex-col items-center justify-center ${statusColors[status]} px-4 border-l border-b border-gray-800 shadow-[1px_1px_2px_rgba(0,0,0,0.3)]`}>
            <div className="text-xl mb-1"><Icon className="w-5 h-5" /></div>
            <div className="text-[10px] font-medium">{label}</div>
          </div>
        </TooltipTrigger>
        <TooltipContent>
          <p>{tooltipText}</p>
        </TooltipContent>
      </Tooltip>
    </TooltipProvider>
  );
};

interface StatusBarProps {
  indicators?: Array<{
    icon: keyof typeof iconComponents;
    label: string;
    status: "normal" | "warning" | "error";
    tooltipText: string;
  }>;
}

const iconComponents = {
  Battery,
  Signal,
  Wifi,
  AlertTriangle,
  Power,
  Server,
  Activity,
};

const defaultIndicators = [
  {
    icon: "Battery",
    label: "L",
    status: "normal",
    tooltipText: "Left battery: OK",
  },
  {
    icon: "Battery",
    label: "R",
    status: "normal",
    tooltipText: "Right battery: OK",
  },
  {
    icon: "Activity",
    label: "0",
    status: "normal",
    tooltipText: "Activity level: 0",
  },
  {
    icon: "AlertTriangle",
    label: "!",
    status: "error",
    tooltipText: "Warning: System alert",
  },
  {
    icon: "Server",
    label: "R",
    status: "normal",
    tooltipText: "Right system: OK",
  },
  {
    icon: "Power",
    label: "*",
    status: "normal",
    tooltipText: "System power: OK",
  },
  {
    icon: "Signal",
    label: "20%",
    status: "normal",
    tooltipText: "Signal strength: 20%",
  },
];

const StatusBar = ({ indicators = defaultIndicators }: StatusBarProps) => {
  return (
    <div className="w-full h-12 bg-neutral-900 border-none flex items-center justify-between px-6 mr-0 mt-2">
      <div className="flex items-center gap-4">
        {indicators.map((indicator, index) => {
          const IconComponent = iconComponents[indicator.icon];
          return (
            <StatusIndicator
              key={index}
              icon={IconComponent}
              label={indicator.label}
              status={indicator.status}
              tooltipText={indicator.tooltipText}
            />
          );
        })}
        
        {/* Animated heartbeat indicator */}
        <div className="relative w-4 h-4 ml-4 border-gray-800 shadow-[1px_1px_2px_rgba(0,0,0,0.3)]">
          <div className="absolute w-full h-full bg-red-500 rounded-full animate-ping opacity-75" />
          <div className="absolute w-full h-full bg-red-500 rounded-full" />
        </div>
      </div>
    </div>
  );
};

export default StatusBar;
