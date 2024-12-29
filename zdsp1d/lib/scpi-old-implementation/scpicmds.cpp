// kommando knoten  für zdsp1 hardware server
// es werden alle kommando knoten für zhserver nodes,
// scpi nodes, commands und , queries für den 
// bearbeitenden kommando interpreter definiert

#include "scpi-zdsp.h" // für scpi knoten
#include "scpicmds.h" // scpi erweiterung

// die vollständige scpi kommando liste

cNodeSCPI* System;
                   cNodeSCPI* SystemCommunication;
                            cNodeSCPI* SystemCommunicationEncryption;
	     cNodeSCPI* SystemDsp;
                            cNodeSCPI* SystemDspTest;
		          cNodeSCPI* SystemDspSampling;	
		          cNodeSCPI* SystemDspTrigger;
			              cNodeSCPI* SystemDspTriggerIntList;
				                   cNodeSCPI* SystemDspTriggerALL;
			          	                   cNodeSCPI* SystemDspTriggerHKSK;
		          cNodeSCPI* SystemDspCommand;
			              cNodeSCPI* SystemDspCommandStat;


cNodeSCPI* Status;
                   cNodeSCPI* StatusDevice;
	     cNodeSCPI* StatusDsp;	   
		          cNodeSCPI* StatusDspLoad;	   
	                                           cNodeSCPI* StatusDspLoadActual;
		                             cNodeSCPI* StatusDspLoadMaximum;
			                                  cNodeSCPI* StatusDspLoadMaximumReset;
	     
cNodeSCPI* MeasureNode;
                  cNodeSCPI* MeasureList;
		          cNodeSCPI* MeasureListCycList;
		          cNodeSCPI* MeasureListIntList;
		          cNodeSCPI* MeasureListRavList;
		          cNodeSCPI* MeasureListSet;
		          cNodeSCPI* MeasureListClear;	  


cNodeSCPI* Memory;	     
	     cNodeSCPI* MemoryRead;
	     cNodeSCPI* MemoryWrite;
	     
	     
// cNodeScpi (QString,tNodeSpec,cNode*,cNode*,SCPICmdType,SCPICmdType); 
// konstruktor, sNodeName, nNodedef, pNextNode, pNewLevelNode, Cmd, Query				
// konstruktor, psNodeNames,psNode2Set, nNodedef, pNextNode, pNewLevelNode, Cmd, Query

cNode* InitCmdTree()
{
    
    // implementiertes memory modell
    
    MemoryWrite=new cNodeSCPI("WRITE",isCommand,NULL,NULL,DspMemoryWrite,nixCmd);	     
    MemoryRead=new cNodeSCPI("READ",isCommand,MemoryWrite,NULL,DspMemoryRead,nixCmd);	     
    Memory=new cNodeSCPI("MEMORY",isNode,NULL,MemoryRead,nixCmd,nixCmd);	     

    // implementiertes measure modell

    MeasureListClear=new cNodeSCPI("CLEAR",isCommand,NULL,NULL,UnloadCmdList,nixCmd);
    MeasureListSet=new cNodeSCPI("SET",isCommand,MeasureListClear,NULL,LoadCmdList,nixCmd);
    MeasureListRavList=new cNodeSCPI("RAVLIST",isQuery | isCommand,MeasureListSet,NULL,SetRavList,GetRavList);
    MeasureListIntList=new cNodeSCPI("INTLIST",isQuery | isCommand,MeasureListRavList,NULL,SetCmdIntList,GetCmdIntList);
    MeasureListCycList=new cNodeSCPI("CYCLIST",isQuery | isCommand,MeasureListIntList,NULL,SetCmdList,GetCmdList);
    MeasureList=new cNodeSCPI("LIST",isNode,NULL,MeasureListCycList,nixCmd,nixCmd);
    MeasureNode=new cNodeSCPI("MEASURE",isNode | isCommand,Memory,MeasureList,Measure,nixCmd);	     
    // implementiertes status modell
    
    StatusDspLoadMaximumReset=new cNodeSCPI("RESET",isCommand,NULL,NULL,ResetDeviceLoadMax,nixCmd);
    StatusDspLoadMaximum=new cNodeSCPI("MAXIMUM",isNode | isQuery,NULL,StatusDspLoadMaximumReset,nixCmd,GetDeviceLoadMax);	
    StatusDspLoadActual=new cNodeSCPI("ACTUAL",isQuery,StatusDspLoadMaximum,NULL,nixCmd,GetDeviceLoadAct);
    StatusDspLoad=new cNodeSCPI("LOAD",isNode,NULL,StatusDspLoadActual,nixCmd,nixCmd);
    StatusDsp=new cNodeSCPI("DSP",isNode | isQuery,NULL,StatusDspLoad,nixCmd,GetDspStatus);
    StatusDevice=new cNodeSCPI("DEVICE",isQuery,StatusDsp,NULL,nixCmd,GetDeviceStatus);
    Status=new cNodeSCPI("STATUS",isNode,MeasureNode,StatusDevice,nixCmd,nixCmd);
    
    // implementiertes system modell
    SystemDspCommandStat=new cNodeSCPI("STAT",isQuery | isCommand,NULL,NULL,SetDspCommandStat,GetDspCommandStat);      
    SystemDspCommand=new cNodeSCPI("COMMAND",isNode,NULL,SystemDspCommandStat,nixCmd,nixCmd);

    SystemDspTriggerHKSK=new cNodeSCPI("HKSK",isCommand,NULL,NULL,TriggerIntListHKSK,nixCmd);	      
    SystemDspTriggerALL=new cNodeSCPI("ALL",isCommand,SystemDspTriggerHKSK,NULL,TriggerIntListALL,nixCmd);
    SystemDspTriggerIntList=new cNodeSCPI("INTLIST",isNode,NULL,SystemDspTriggerALL,nixCmd,nixCmd);
    SystemDspTrigger=new cNodeSCPI("TRIGGER",isNode,SystemDspCommand,SystemDspTriggerIntList,nixCmd,nixCmd);
    SystemDspSampling=new cNodeSCPI("SAMPLING",isQuery | isCommand,SystemDspTrigger,NULL,SetSamplingSystem,GetSamplingSystem);
    SystemDspTest=new cNodeSCPI("TEST",isCommand,SystemDspSampling,NULL,TestDsp,nixCmd);
    SystemDsp=new cNodeSCPI("DSP",isNode,NULL,SystemDspTest,nixCmd,nixCmd);
    SystemCommunicationEncryption=new cNodeSCPI("ENCRYPTION",isQuery | isCommand,NULL,NULL,SetCommEncryption,GetCommEncryption);
    SystemCommunication=new cNodeSCPI("COMMUNICATION",isNode,SystemDsp,SystemCommunicationEncryption,nixCmd,nixCmd);
    System=new cNodeSCPI("SYSTEM",isNode,Status,SystemCommunication,nixCmd,nixCmd);
    return (System);  
}
