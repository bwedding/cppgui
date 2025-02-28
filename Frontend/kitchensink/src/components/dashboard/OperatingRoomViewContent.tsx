import React, { useMemo, useEffect, useState } from "react";
import { useAtomValue } from "jotai";
import { 
  sensorReadingsSignal as mainSensorReadingsSignal,
  atrialPressuresSignal as mainAtrialPressuresSignal,
  cardiacOutputSignal as mainCardiacOutputSignal,
  pressureCardVisibilitySignal as mainPressureCardVisibilitySignal
} from "@/lib/datastore";
import { 
  sensorReadingsSignal as subsetSensorReadingsSignal,
  atrialPressuresSignal as subsetAtrialPressuresSignal,
  cardiacOutputSignal as subsetCardiacOutputSignal,
  pressureCardVisibilitySignal as subsetPressureCardVisibilitySignal
} from "@/lib/subset-datastore";
import { useSignals } from '@preact/signals-react/runtime';
import PressureCard from "./PressureCard";
import ClockStopwatch from "./Clock";
import GridLayout from "react-grid-layout";
import "react-grid-layout/css/styles.css";
import "react-resizable/css/styles.css";

// Move parsing function outside component to prevent recreation
const parseValue = (value: any): number => {
  if (value === undefined || value === null || value === 'NaN') return 0;
  if (typeof value === 'number') return isNaN(value) ? 0 : value;
  const parsed = parseFloat(value.toString().replace(/['"]+/g, ''));
  return isNaN(parsed) ? 0 : parsed;
};

interface OperatingRoomContentProps {
  isSubset?: boolean;
}

const OperatingRoomContent: React.FC<OperatingRoomContentProps> = ({ isSubset = false }) => {
  // Enable signals reactivity
  useSignals();
  
  // Use the appropriate signals based on isSubset prop
  const atrialPressuresSignal = isSubset ? subsetAtrialPressuresSignal : mainAtrialPressuresSignal;
  const cardiacOutputSignal = isSubset ? subsetCardiacOutputSignal : mainCardiacOutputSignal;
  const pressureCardVisibilitySignal = isSubset ? subsetPressureCardVisibilitySignal : mainPressureCardVisibilitySignal;

  // Read data from signals
  const sensorReadings = isSubset 
    ? subsetSensorReadingsSignal.value
    : mainSensorReadingsSignal.value;
  const atrialPressures = atrialPressuresSignal.value;
  const cardiacOutput = cardiacOutputSignal.value;
  const pressureCardVisibility = pressureCardVisibilitySignal.value;

  // Memoize all card configurations in a single array
  const allCards = useMemo(() => [
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
    const savedLayout = localStorage.getItem('viewCardsLayout');
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
        static: false // Allow the user to move cards
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
    localStorage.setItem('viewCardsLayout', JSON.stringify(constrainedLayout));
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
      localStorage.setItem('viewCardsLayout', JSON.stringify(filteredLayout));
    }
  }, [visibleCards, layout]);

  // Calculate the container width based on the viewport
  const [containerWidth, setContainerWidth] = useState(1800);
  
  useEffect(() => {
    const updateWidth = () => {
      // Use the window width minus some padding, but ensure it's responsive
      const calculatedWidth = Math.max(800, window.innerWidth - 50);
      setContainerWidth(calculatedWidth);
    };
    
    updateWidth();
    window.addEventListener('resize', updateWidth);
    return () => window.removeEventListener('resize', updateWidth);
  }, []);

  // Calculate the cell width based on the container width, columns, and margins
  const cellWidth = useMemo(() => {
    const totalMarginWidth = (5 - 1) * 20; // (cols - 1) * margin
    const availableWidth = containerWidth - totalMarginWidth - (2 * 20); // Subtract container padding
    return Math.floor(availableWidth / 5); // Divide by number of columns
  }, [containerWidth]);

  return (
    <div className="space-y-6 bg-neutral-800 relative z-0">
      <div className="flex-1 overflow-auto">
        <div className="p-4 h-full">
          <GridLayout
            className="layout"
            layout={layout}
            cols={5}
            rowHeight={cellWidth}
            width={containerWidth}
            isResizable={false}
            onLayoutChange={handleLayoutChange}
            compactType="vertical"
            maxRows={2}
            margin={[10, 10]}
            containerPadding={[10, 10]}
            draggableHandle=".drag-handle"
          >
            {visibleCards.map(card => (
              <div key={card.metricId} className="h-full w-full">
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
};

export default OperatingRoomContent;