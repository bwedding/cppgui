import React from 'react';
import ReactDOM from 'react-dom/client';
import { Provider, useAtom } from 'jotai';
import { 
  sensorReadingsAtom, 
  leftHeartAtom, 
  rightHeartAtom,
  stopwatchAtom,
  initializeMessageHandling 
} from './lib/subset-datastore';
import SubsetOperatingRoomView from './components/dashboard/SubsetOperatingRoomView'
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

// Simple wrapper component
const SubsetApp: React.FC = () => {
  return (
    <div className="min-h-screen bg-neutral-800">
      <SubsetOperatingRoomView />
    </div>
  );
};

ReactDOM.createRoot(document.getElementById('root')!).render(
  <React.StrictMode>
    <Provider>
      <MessageHandler />
      <SubsetApp />
    </Provider>
  </React.StrictMode>
);