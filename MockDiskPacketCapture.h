#pragma once

#include "DiskPacketCapture.h"
#include <string>

class MockDiskPacketCapture : public DiskPacketCapture {
public:

   MockDiskPacketCapture(Conf& conf) : DiskPacketCapture(conf) {
   }

   virtual ~MockDiskPacketCapture() {
   }
   bool Initialize(const bool diskCleanThread = false) {
      return DiskPacketCapture::Initialize(diskCleanThread);
   }
   void GetRunningPackets(const std::string& uuid, std::pair<InMemoryPacketBuffer*, size_t>*& sessionInfo) {
      DiskPacketCapture::GetRunningPackets(uuid,sessionInfo);
   }

   void RemoveOldestPCapFile() {
      DiskPacketCapture::RemoveOldestPCapFiles(1);
   }
   void RemoveOldestPCapFiles(const size_t max) {
      DiskPacketCapture::RemoveOldestPCapFiles(1);
   }
   void RemoveFromRunningPackets(const std::string& uuid) {
      DiskPacketCapture::RemoveFromRunningPackets(uuid);
   }

   bool TooMuchPCap(std::atomic<size_t>& aDiskUsed, std::atomic<size_t>& aTotalFiles) {
      return DiskPacketCapture::TooMuchPCap(aDiskUsed, aTotalFiles);
   }

   std::string BuildFilename( const std::string& uuid,
           const std::string& appName, const std::time_t time) {
      return DiskPacketCapture::BuildFilename( uuid, appName, time);
   }

   int NewTotalMemory(const size_t memoryAddedIfSaved) {
      return DiskPacketCapture::NewTotalMemory(memoryAddedIfSaved);
   }

   int CurrentMemoryForFlow(const std::string& uuid) {
      return DiskPacketCapture::CurrentMemoryForFlow(uuid);
   }
   void RecalculateDiskUsed(std::atomic<size_t>& aDiskUsed, std::atomic<size_t>& aTotalFiles) {
      DiskPacketCapture::RecalculateDiskUsed(aDiskUsed, aTotalFiles);
   }
   void CleanupOldPcapFiles(std::atomic<size_t>& aDiskUsed, std::atomic<size_t>& aTotalFiles) {
      DiskPacketCapture::CleanupOldPcapFiles(aDiskUsed, aTotalFiles);
   }
};

