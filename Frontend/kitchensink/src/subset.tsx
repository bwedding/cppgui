// src/subset.tsx
import React from 'react';
import ReactDOM from 'react-dom/client';
import { Provider, useAtom } from 'jotai';
import { 
  sensorReadingsAtom, 
  leftHeartAtom, 
  rightHeartAtom,
  stopwatchAtom,
  initializeMessageHandling 
} from './lib/subset-datastore.ts';
import SubsetLayout from './components/SubsetLayout.tsx';
import SubsetOperatingRoomView from './components/dashboard/SubsetOperatingRoomView.tsx';
import './index.css';

// Component to initialize message handling
const MessageHandler: React.FC = () => {
  const [, setSensorReadings] = useAtom(sensorReadingsAtom);
  const [, setLeftHeart] = useAtom(leftHeartAtom);
  const [, setRightHeart] = useAtom(rightHeartAtom);
  const [, setStopwatch] = useAtom(stopwatchAtom);

  React.useEffect(() => {
    initializeMessageHandling({
      setSensorReadings,
      setLeftHeart,
      setRightHeart,
      setStopwatch
    });
  }, []);

  return null;
};

ReactDOM.createRoot(document.getElementById('root')!).render(
  <React.StrictMode>
    <Provider>
      <MessageHandler />
      <SubsetLayout>
        <SubsetOperatingRoomView />
      </SubsetLayout>
    </Provider>
  </React.StrictMode>
);