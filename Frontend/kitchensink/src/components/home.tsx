import React, { useState } from "react";
import MetricsGrid from "./dashboard/MetricsGrid.tsx";
import DashboardTabs from "./dashboard/DashboardTabs.tsx";
import TitleBar from "./dashboard/titlebar.tsx";
import Footer from "./dashboard/Footer.tsx";
import OperatingRoomView from "./dashboard/OperatingRoomView.tsx";
import { useAtomValue } from "jotai";
import { 
  sensorReadingsAtom, 
  leftHeartAtom,
  rightHeartAtom,
  heartMetricsAtom 
} from "lib/datastore";

interface HomeProps {
  initialMode?: "auto" | "manual";
}

const Home = ({ initialMode = "auto" }: HomeProps) => {
  const [activeTab, setActiveTab] = useState("technical");

  const TechnicalView = (
    <div className="bg-neutral-800 mx-0">
      <div className="bg-neutral-800 max-w-[2200px] mx-auto">
        <MetricsGrid />
      </div>
    </div>
  );

  return (
    <div className="min-h-screen w-full bg-neutral-800 text-white flex flex-col">
      <TitleBar />
      <div className="flex-1 px-4">
        <DashboardTabs
          activeTab={activeTab}
          onTabChange={setActiveTab}
          technicalContent={TechnicalView}
          operatingContent={<OperatingRoomView />}
        />
      </div>
      <Footer />
    </div>
  );
};

export default Home;
