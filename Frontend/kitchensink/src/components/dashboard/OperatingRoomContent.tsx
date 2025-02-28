import React, { useMemo, useEffect, useState, useCallback } from "react";
import { useAtomValue } from "jotai";
import { 
  atrialPressuresSignal as mainAtrialPressuresSignal,
  cardiacOutputSignal as mainCardiacOutputSignal,
  pressureCardVisibilitySignal as mainPressureCardVisibilitySignal,
  sensorReadingsSignal as mainSensorReadingsSignal
} from "@/lib/datastore";
import { 
  atrialPressuresSignal as subsetAtrialPressuresSignal,
  cardiacOutputSignal as subsetCardiacOutputSignal,
  pressureCardVisibilitySignal as subsetPressureCardVisibilitySignal,
  sensorReadingsSignal as subsetSensorReadingsSignal
} from "@/lib/subset-datastore";
import { useSignals } from '@preact/signals-react/runtime';
import PressureCard from "./PressureCard";
import ClockStopwatch from "./Clock";
import GridLayout from "react-grid-layout";
import "react-grid-layout/css/styles.css";
import "react-resizable/css/styles.css";

// Define types for our data structures
interface Reading {
  MaxValue?: number | string;
  MinValue?: number | string;
  MeanValue?: number | string;
}

interface SensorReadings {
  artPress?: Reading;
  pap?: Reading;
  cvp?: Reading;
  aop?: Reading;
  ivc?: Reading;
}

interface AtrialPressures {
  left?: Reading;
  right?: Reading;
}

interface CardiacOutput {
  left?: Reading;
  right?: Reading;
}

interface PressureCardVisibility {
  MAP: boolean;
  PAP: boolean;
  CVP: boolean;
  AOP: boolean;
  LAP: boolean;
  RAP: boolean;
  IVC: boolean;
  LCO: boolean;
  RCO: boolean;
}

interface CardConfig {
  title: string;
  metricId: keyof PressureCardVisibility | "clock";
}

// Move parsing function outside component and memoize
const parseValue = (value: any): number => {
  if (value === undefined || value === null || value === 'NaN') return 0;
  if (typeof value === 'number') return isNaN(value) ? 0 : value;
  const parsed = parseFloat(value.toString().replace(/['"]+/g, ''));
  return isNaN(parsed) ? 0 : parsed;
};

// Individual card components that subscribe to their specific signals
const ClockCard = React.memo(() => {
  return <ClockStopwatch />;
});
ClockCard.displayName = 'ClockCard';

// MAP Card
const MAPCard = React.memo(({ isSubset }: { isSubset: boolean }) => {
  useSignals();
  const sensorReadingsSignal = isSubset ? subsetSensorReadingsSignal : mainSensorReadingsSignal;
  const pressureCardVisibilitySignal = isSubset ? subsetPressureCardVisibilitySignal : mainPressureCardVisibilitySignal;
  
  const readings = sensorReadingsSignal.value.artPress;
  const visible = pressureCardVisibilitySignal.value.MAP;
  
  if (!visible) return null;
  
  return (
    <PressureCard
      title="MAP"
      value1={parseValue(readings?.MaxValue)}
      value2={parseValue(readings?.MinValue)}
      mean={parseValue(readings?.MeanValue)}
      metricId="MAP"
      visible={visible}
    />
  );
});
MAPCard.displayName = 'MAPCard';

// PAP Card
const PAPCard = React.memo(({ isSubset }: { isSubset: boolean }) => {
  useSignals();
  const sensorReadingsSignal = isSubset ? subsetSensorReadingsSignal : mainSensorReadingsSignal;
  const pressureCardVisibilitySignal = isSubset ? subsetPressureCardVisibilitySignal : mainPressureCardVisibilitySignal;
  
  const readings = sensorReadingsSignal.value.pap;
  const visible = pressureCardVisibilitySignal.value.PAP;
  
  if (!visible) return null;
  
  return (
    <PressureCard
      title="PAP"
      value1={parseValue(readings?.MaxValue)}
      value2={parseValue(readings?.MinValue)}
      mean={parseValue(readings?.MeanValue)}
      metricId="PAP"
      visible={visible}
    />
  );
});
PAPCard.displayName = 'PAPCard';

// CVP Card
const CVPCard = React.memo(({ isSubset }: { isSubset: boolean }) => {
  useSignals();
  const sensorReadingsSignal = isSubset ? subsetSensorReadingsSignal : mainSensorReadingsSignal;
  const pressureCardVisibilitySignal = isSubset ? subsetPressureCardVisibilitySignal : mainPressureCardVisibilitySignal;
  
  const readings = sensorReadingsSignal.value.cvp;
  const visible = pressureCardVisibilitySignal.value.CVP;
  
  if (!visible) return null;
  
  return (
    <PressureCard
      title="CVP"
      value1={parseValue(readings?.MaxValue)}
      value2={parseValue(readings?.MinValue)}
      mean={parseValue(readings?.MeanValue)}
      metricId="CVP"
      visible={visible}
    />
  );
});
CVPCard.displayName = 'CVPCard';

// AOP Card
const AOPCard = React.memo(({ isSubset }: { isSubset: boolean }) => {
  useSignals();
  const sensorReadingsSignal = isSubset ? subsetSensorReadingsSignal : mainSensorReadingsSignal;
  const pressureCardVisibilitySignal = isSubset ? subsetPressureCardVisibilitySignal : mainPressureCardVisibilitySignal;
  
  const readings = sensorReadingsSignal.value.aop;
  const visible = pressureCardVisibilitySignal.value.AOP;
  
  if (!visible) return null;
  
  return (
    <PressureCard
      title="AoP"
      value1={parseValue(readings?.MaxValue)}
      value2={parseValue(readings?.MinValue)}
      mean={parseValue(readings?.MeanValue)}
      metricId="AOP"
      visible={visible}
    />
  );
});
AOPCard.displayName = 'AOPCard';

// LAP Card
const LAPCard = React.memo(({ isSubset }: { isSubset: boolean }) => {
  useSignals();
  const atrialPressuresSignal = isSubset ? subsetAtrialPressuresSignal : mainAtrialPressuresSignal;
  const pressureCardVisibilitySignal = isSubset ? subsetPressureCardVisibilitySignal : mainPressureCardVisibilitySignal;
  
  const readings = atrialPressuresSignal.value.left;
  const visible = pressureCardVisibilitySignal.value.LAP;
  
  if (!visible) return null;
  
  return (
    <PressureCard
      title="lAP"
      value1={parseValue(readings?.MaxValue)}
      value2={parseValue(readings?.MinValue)}
      mean={parseValue(readings?.MeanValue)}
      metricId="LAP"
      visible={visible}
    />
  );
});
LAPCard.displayName = 'LAPCard';

// RAP Card
const RAPCard = React.memo(({ isSubset }: { isSubset: boolean }) => {
  useSignals();
  const atrialPressuresSignal = isSubset ? subsetAtrialPressuresSignal : mainAtrialPressuresSignal;
  const pressureCardVisibilitySignal = isSubset ? subsetPressureCardVisibilitySignal : mainPressureCardVisibilitySignal;
  
  const readings = atrialPressuresSignal.value.right;
  const visible = pressureCardVisibilitySignal.value.RAP;
  
  if (!visible) return null;
  
  return (
    <PressureCard
      title="rAP"
      value1={parseValue(readings?.MaxValue)}
      value2={parseValue(readings?.MinValue)}
      mean={parseValue(readings?.MeanValue)}
      metricId="RAP"
      visible={visible}
    />
  );
});
RAPCard.displayName = 'RAPCard';

// IVC Card
const IVCCard = React.memo(({ isSubset }: { isSubset: boolean }) => {
  useSignals();
  const sensorReadingsSignal = isSubset ? subsetSensorReadingsSignal : mainSensorReadingsSignal;
  const pressureCardVisibilitySignal = isSubset ? subsetPressureCardVisibilitySignal : mainPressureCardVisibilitySignal;
  
  const readings = sensorReadingsSignal.value.ivc;
  const visible = pressureCardVisibilitySignal.value.IVC;
  
  if (!visible) return null;
  
  return (
    <PressureCard
      title="IVC"
      value1={parseValue(readings?.MaxValue)}
      value2={parseValue(readings?.MinValue)}
      mean={parseValue(readings?.MeanValue)}
      metricId="IVC"
      visible={visible}
    />
  );
});
IVCCard.displayName = 'IVCCard';

// LCO Card
const LCOCard = React.memo(({ isSubset }: { isSubset: boolean }) => {
  useSignals();
  const cardiacOutputSignal = isSubset ? subsetCardiacOutputSignal : mainCardiacOutputSignal;
  const pressureCardVisibilitySignal = isSubset ? subsetPressureCardVisibilitySignal : mainPressureCardVisibilitySignal;
  
  const readings = cardiacOutputSignal.value.left;
  const visible = pressureCardVisibilitySignal.value.LCO;
  
  if (!visible) return null;
  
  return (
    <PressureCard
      title="lCO"
      value1={parseValue(readings?.MaxValue)}
      value2={parseValue(readings?.MinValue)}
      mean={parseValue(readings?.MeanValue)}
      metricId="LCO"
      visible={visible}
    />
  );
});
LCOCard.displayName = 'LCOCard';

// RCO Card
const RCOCard = React.memo(({ isSubset }: { isSubset: boolean }) => {
  useSignals();
  const cardiacOutputSignal = isSubset ? subsetCardiacOutputSignal : mainCardiacOutputSignal;
  const pressureCardVisibilitySignal = isSubset ? subsetPressureCardVisibilitySignal : mainPressureCardVisibilitySignal;
  
  const readings = cardiacOutputSignal.value.right;
  const visible = pressureCardVisibilitySignal.value.RCO;
  
  if (!visible) return null;
  
  return (
    <PressureCard
      title="rCO"
      value1={parseValue(readings?.MaxValue)}
      value2={parseValue(readings?.MinValue)}
      mean={parseValue(readings?.MeanValue)}
      metricId="RCO"
      visible={visible}
    />
  );
});
RCOCard.displayName = 'RCOCard';

interface OperatingRoomContentProps {
  isSubset?: boolean;
}

const OperatingRoomContent: React.FC<OperatingRoomContentProps> = React.memo(({ isSubset = false }) => {
  // Enable signals reactivity for the visibility state
  useSignals();
  
  // Use the appropriate signals based on isSubset prop
  const pressureCardVisibilitySignal = isSubset ? subsetPressureCardVisibilitySignal : mainPressureCardVisibilitySignal;
  const pressureCardVisibility = pressureCardVisibilitySignal.value;

  // Define static card configurations
  const cardConfigs = useMemo<CardConfig[]>(() => [
    { title: "Clock", metricId: "clock" },
    { title: "MAP", metricId: "MAP" },
    { title: "PAP", metricId: "PAP" },
    { title: "CVP", metricId: "CVP" },
    { title: "AoP", metricId: "AOP" },
    { title: "lAP", metricId: "LAP" },
    { title: "rAP", metricId: "RAP" },
    { title: "IVC", metricId: "IVC" },
    { title: "lCO", metricId: "LCO" },
    { title: "rCO", metricId: "RCO" },
  ], []);

  // Memoize filtered cards based on visibility
  const visibleCardConfigs = useMemo(() => 
    cardConfigs.filter(card => 
      card.metricId === "clock" || pressureCardVisibility[card.metricId as keyof PressureCardVisibility]
    ),
    [cardConfigs, pressureCardVisibility]
  );

  // Initialize and manage layout state
  const [layout, setLayout] = useState(() => {
    const savedLayout = localStorage.getItem('cardsLayout');
    if (savedLayout) {
      try {
        return JSON.parse(savedLayout);
      } catch (e) {
        console.error('Failed to parse saved layout', e);
      }
    }
    
    // Default layout if nothing is saved - arrange in a grid pattern
    return visibleCardConfigs.map((card, i) => {
      const row = Math.floor(i / 5); // 5 cards per row
      const col = i % 5;
      return {
        i: card.metricId,
        x: col,
        y: row,
        w: 1,
        h: 1,
      };
    });
  });

  // Save layout changes to localStorage - optimize with useCallback
  const handleLayoutChange = useCallback((newLayout: any) => {
    // Constrain all items to stay within the first two rows (y=0 or y=1)
    const constrainedLayout = newLayout.map((item: any) => ({
      ...item,
      y: Math.min(item.y, 1) // Limit y to 0 or 1 (first two rows)
    }));
    
    setLayout(constrainedLayout);
    localStorage.setItem('cardsLayout', JSON.stringify(constrainedLayout));
  }, []);

  // Update layout when visible cards change
  useEffect(() => {
    const currentIds = new Set(layout.map((item: any) => item.i));
    const visibleIds = new Set(visibleCardConfigs.map(card => card.metricId));
    
    let layoutChanged = false;
    const newLayout = [...layout];
    
    // Add new visible cards not in current layout
    visibleCardConfigs.forEach((card, index) => {
      if (!currentIds.has(card.metricId)) {
        // Find an empty spot in the first two rows
        // Count existing items in each row
        const itemsInRow0 = newLayout.filter(item => item.y === 0).length;
        const itemsInRow1 = newLayout.filter(item => item.y === 1).length;
        
        // Place in row with fewer items
        const row = itemsInRow0 <= itemsInRow1 ? 0 : 1;
        // Place at the end of the selected row
        const itemsInSelectedRow = row === 0 ? itemsInRow0 : itemsInRow1;
        
        newLayout.push({
          i: card.metricId,
          x: itemsInSelectedRow % 5, // Position in the row
          y: row, // Either row 0 or 1
          w: 1,
          h: 1,
        });
        layoutChanged = true;
      }
    });
    
    // Remove cards that are no longer visible
    const filteredLayout = newLayout.filter(item => visibleIds.has(item.i));
    if (filteredLayout.length !== newLayout.length) {
      layoutChanged = true;
    }
    
    if (layoutChanged) {
      setLayout(filteredLayout);
      localStorage.setItem('cardsLayout', JSON.stringify(filteredLayout));
    }
  }, [visibleCardConfigs, layout]);

  // Calculate the container width based on the viewport
  const [containerWidth, setContainerWidth] = useState(1800);
  
  // Optimize resize handler with useCallback
  const updateWidth = useCallback(() => {
    // Use the window width minus some padding
    setContainerWidth(Math.max(1200, window.innerWidth - 100));
  }, []);
  
  useEffect(() => {
    updateWidth();
    window.addEventListener('resize', updateWidth);
    return () => window.removeEventListener('resize', updateWidth);
  }, [updateWidth]);

  // Memoize the GridLayout configuration
  const gridConfig = useMemo(() => ({
    className: "layout",
    layout: layout,
    cols: 5,
    rowHeight: 400,
    width: containerWidth,
    isResizable: false,
    onLayoutChange: handleLayoutChange,
    compactType: "vertical",
    maxRows: 2,
    margin: [15, 15]
  }), [layout, containerWidth, handleLayoutChange]);

  // Render the appropriate card component based on metricId
  const renderCardComponent = useCallback((metricId: string) => {
    switch(metricId) {
      case "clock": return <ClockCard />;
      case "MAP": return <MAPCard isSubset={isSubset} />;
      case "PAP": return <PAPCard isSubset={isSubset} />;
      case "CVP": return <CVPCard isSubset={isSubset} />;
      case "AOP": return <AOPCard isSubset={isSubset} />;
      case "LAP": return <LAPCard isSubset={isSubset} />;
      case "RAP": return <RAPCard isSubset={isSubset} />;
      case "IVC": return <IVCCard isSubset={isSubset} />;
      case "LCO": return <LCOCard isSubset={isSubset} />;
      case "RCO": return <RCOCard isSubset={isSubset} />;
      default: return null;
    }
  }, [isSubset]);

  return (
    <div className="space-y-6 bg-neutral-800 relative z-0">
      <div className="flex-1 overflow-auto">
        <div className="p-4 h-full">
          <GridLayout
            {...gridConfig}
          >
            {visibleCardConfigs.map(card => (
              <div key={card.metricId} className="min-h-0 flex-1">
                {renderCardComponent(card.metricId)}
              </div>
            ))}
          </GridLayout>
        </div>
      </div>
    </div>
  );
});

OperatingRoomContent.displayName = 'OperatingRoomContent';

export default OperatingRoomContent;