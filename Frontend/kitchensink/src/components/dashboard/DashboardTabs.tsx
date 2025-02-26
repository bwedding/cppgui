import { Tabs, TabsList, TabsTrigger, TabsContent } from "@/components/ui/tabs.tsx";
import { type ReactNode, Suspense } from "react";
import StatusBar from "@/components/dashboard/StatusBar.tsx"; // Assuming StatusBar is imported from this location

interface DashboardTabsProps {
  activeTab: string;
  onTabChange: (value: string) => void;
  children?: ReactNode;
  technicalContent?: ReactNode;
  operatingContent?: ReactNode;
}

const DashboardTabs = ({
  activeTab,
  onTabChange,
  technicalContent,
  operatingContent,
}: DashboardTabsProps) => {
  const handleDeferredTabChange = (value: string) => {
    // Immediate UI feedback
    onTabChange(value);

    // Defer heavy content updates
    setTimeout(() => {
      // Let React batch any heavy state updates that happen
      // when tab content mounts/unmounts
      window.requestAnimationFrame(() => {
        // This will run after the tab switch animation
      });
    }, 0);
  };
  
  return (
    <Tabs value={activeTab} onValueChange={handleDeferredTabChange} className="w-full">
      <TabsList className="bg-neutral-900 flex justify-between mb-1 mt-4 ml-4 pb-2 h-[50px]">
        <div className="flex">
          <TabsTrigger
            value="technical"
            className="-mx-3 px-4 py-8 -mb-[1px] text-lg text-gray-400 data-[state=active]:text-white data-[state=active]:bg-neutral-800 rounded-none"
          >
            Technical Board
          </TabsTrigger>
          <TabsTrigger
            value="operating"
            className="mx-2 px-4 py-8 -mb-[1px] text-lg text-gray-400 data-[state=active]:text-white data-[state=active]:bg-neutral-800 rounded-none"
          >
            Operating Room View
          </TabsTrigger>
        </div>
        <div className="flex-shrink-0">
          <StatusBar />
        </div>
      </TabsList>
      <Suspense fallback={<div>Loading...</div>}>
       <TabsContent value="technical">
         {activeTab === 'technical' && technicalContent}
       </TabsContent>
       <TabsContent value="operating">
         {activeTab === 'operating' && operatingContent} 
       </TabsContent>
     </Suspense>
    </Tabs>
  );
};

export default DashboardTabs;
