import React, { useMemo, useEffect, useState } from "react";
import { useAtomValue } from "jotai";
import { 
  sensorReadingsAtom as mainSensorReadingsAtom,
  AtrialPressuresAtom as mainAtrialPressuresAtom,
  CardiacOutputAtom as mainCardiacOutputAtom,
  pressureCardVisibilityAtom as mainPressureCardVisibilityAtom
} from "@/lib/datastore";

import {
  sensorReadingsAtom as subsetSensorReadingsAtom,
  AtrialPressuresAtom as subsetAtrialPressuresAtom,
  CardiacOutputAtom as subsetCardiacOutputAtom,
  pressureCardVisibilityAtom as subsetPressureCardVisibilityAtom
} from "@/lib/subset-datastore";

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
  readings?: Reading;
  metricId: keyof PressureCardVisibility | "clock";
  visible: boolean;
}

// Move parsing function outside component and memoize
const parseValue = (value: any): number => {
  if (value === undefined || value === null || value === 'NaN') return 0;
  if (typeof value === 'number') return isNaN(value) ? 0 : value;
  const parsed = parseFloat(value.toString().replace(/['"]+/g, ''));
  return isNaN(parsed) ? 0 : parsed;
};

interface OperatingRoomContentProps {
  isSubset?: boolean;
}

const OperatingRoomContent: React.FC<OperatingRoomContentProps> = React.memo(({ isSubset = false }) => {
  // Use the appropriate atoms based on isSubset prop
  const sensorReadingsAtom = isSubset ? subsetSensorReadingsAtom : mainSensorReadingsAtom;
  const AtrialPressuresAtom = isSubset ? subsetAtrialPressuresAtom : mainAtrialPressuresAtom;
  const CardiacOutputAtom = isSubset ? subsetCardiacOutputAtom : mainCardiacOutputAtom;
  const pressureCardVisibilityAtom = isSubset ? subsetPressureCardVisibilityAtom : mainPressureCardVisibilityAtom;

  // Read atoms directly without creating derived atoms
  const sensorReadings = useAtomValue(sensorReadingsAtom);
  const atrialPressures = useAtomValue(AtrialPressuresAtom);
  const cardiacOutput = useAtomValue(CardiacOutputAtom);
  const pressureCardVisibility = useAtomValue(pressureCardVisibilityAtom);

  // Memoize all card configurations in a single array
  const allCards = useMemo<CardConfig[]>(() => [
    // Clock (special case)
    {
      title: "Clock",
      metricId: "clock",
      visible: true
    },
    // Top row cards
    {
      title: "MAP",
      readings: sensorReadings.artPress,
      metricId: "MAP",
      visible: pressureCardVisibility.MAP
    },
    {
      title: "PAP",
      readings: sensorReadings.pap,
      metricId: "PAP",
      visible: pressureCardVisibility.PAP
    },
    {
      title: "CVP",
      readings: sensorReadings.cvp,
      metricId: "CVP",
      visible: pressureCardVisibility.CVP
    },
    {
      title: "AoP",
      readings: sensorReadings.aop,
      metricId: "AOP",
      visible: pressureCardVisibility.AOP
    },
    // Bottom row cards
    {
      title: "lAP",
      readings: atrialPressures.left,
      metricId: "LAP",
      visible: pressureCardVisibility.LAP
    },
    {
      title: "rAP",
      readings: atrialPressures.right,
      metricId: "RAP",
      visible: pressureCardVisibility.RAP
    },
    {
      title: "IVC",
      readings: sensorReadings.ivc,
      metricId: "IVC",
      visible: pressureCardVisibility.IVC
    },
    {
      title: "lCO",
      readings: cardiacOutput.left,
      metricId: "LCO",
      visible: pressureCardVisibility.LCO
    },
    {
      title: "rCO",
      readings: cardiacOutput.right,
      metricId: "RCO",
      visible: pressureCardVisibility.RCO
    },
  ], [sensorReadings, atrialPressures, cardiacOutput, pressureCardVisibility]);

  // Memoize filtered cards
  const visibleCards = useMemo(() => 
    allCards.filter(card => card.visible),
    [allCards]
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
    return visibleCards.map((card, i) => {
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

  // Save layout changes to localStorage
  const handleLayoutChange = (newLayout: any) => {
    // Constrain all items to stay within the first two rows (y=0 or y=1)
    const constrainedLayout = newLayout.map((item: any) => ({
      ...item,
      y: Math.min(item.y, 1) // Limit y to 0 or 1 (first two rows)
    }));
    
    setLayout(constrainedLayout);
    localStorage.setItem('cardsLayout', JSON.stringify(constrainedLayout));
  };

  // Update layout when visible cards change
  useEffect(() => {
    const currentIds = new Set(layout.map((item: any) => item.i));
    const visibleIds = new Set(visibleCards.map(card => card.metricId));
    
    let layoutChanged = false;
    const newLayout = [...layout];
    
    // Add new visible cards not in current layout
    visibleCards.forEach((card, index) => {
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
  }, [visibleCards, layout]);

  // Calculate the container width based on the viewport
  const [containerWidth, setContainerWidth] = useState(1800);
  
  useEffect(() => {
    const updateWidth = () => {
      // Use the window width minus some padding
      setContainerWidth(Math.max(1200, window.innerWidth - 100));
    };
    
    updateWidth();
    window.addEventListener('resize', updateWidth);
    return () => window.removeEventListener('resize', updateWidth);
  }, []);

  return (
    <div className="space-y-6 bg-neutral-800 relative z-0">
      <div className="flex-1 overflow-auto">
        <div className="p-4 h-full">
          <GridLayout
            className="layout"
            layout={layout}
            cols={5}
            rowHeight={400}
            width={containerWidth}
            isResizable={false}
            onLayoutChange={handleLayoutChange}
            compactType="vertical"
            maxRows={2}
            margin={[15, 15]}
          >
            {visibleCards.map(card => (
              <div key={card.metricId} className="min-h-0 flex-1">
                {card.metricId === "clock" ? (
                  <ClockStopwatch />
                ) : (
                  <PressureCard
                    title={card.title}
                    value1={parseValue(card.readings?.MaxValue)}
                    value2={parseValue(card.readings?.MinValue)}
                    mean={parseValue(card.readings?.MeanValue)}
                    metricId={card.metricId}
                    visible={card.visible}
                  />
                )}
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