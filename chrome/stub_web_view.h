// Copyright 2013 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_TEST_CHROMEDRIVER_CHROME_STUB_WEB_VIEW_H_
#define CHROME_TEST_CHROMEDRIVER_CHROME_STUB_WEB_VIEW_H_

#include <memory>
#include <string>

#include "chrome/test/chromedriver/chrome/web_view.h"

class StubWebView : public WebView {
 public:
  explicit StubWebView(const std::string& id);
  ~StubWebView() override;

  // Overridden from WebView:
  bool IsServiceWorker() const override;
  std::string GetId() override;
  std::string GetSessionId() override;
  bool WasCrashed() override;
  Status HandleEventsUntil(const ConditionalFunc& conditional_func,
                           const Timeout& timeout) override;
  Status HandleReceivedEvents() override;
  Status GetUrl(std::string* url) override;
  Status Load(const std::string& url, const Timeout* timeout) override;
  Status Reload(const Timeout* timeout) override;
  Status Freeze(const Timeout* timeout) override;
  Status Resume(const Timeout* timeout) override;
  Status StartBidiServer(std::string bidi_mapper_script,
                         bool enable_unsafe_extension_debugging) override;
  Status PostBidiCommand(base::Value::Dict command) override;
  Status SendBidiCommand(base::Value::Dict command,
                         const Timeout& timeout,
                         base::Value::Dict& response) override;
  Status SendCommand(const std::string& cmd,
                     const base::Value::Dict& params) override;
  Status SendCommandFromWebSocket(const std::string& cmd,
                                  const base::Value::Dict& params,
                                  const int client_cmd_id) override;
  Status SendCommandAndGetResult(const std::string& cmd,
                                 const base::Value::Dict& params,
                                 std::unique_ptr<base::Value>* value) override;
  Status TraverseHistory(int delta, const Timeout* timeout) override;
  Status EvaluateScript(const std::string& frame,
                        const std::string& function,
                        const bool await_promise,
                        std::unique_ptr<base::Value>* result) override;
  Status CallFunction(const std::string& frame,
                      const std::string& function,
                      const base::Value::List& args,
                      std::unique_ptr<base::Value>* result) override;
  Status CallUserAsyncFunction(const std::string& frame,
                               const std::string& function,
                               const base::Value::List& args,
                               const base::TimeDelta& timeout,
                               std::unique_ptr<base::Value>* result) override;
  Status CallUserSyncScript(const std::string& frame,
                            const std::string& script,
                            const base::Value::List& args,
                            const base::TimeDelta& timeout,
                            std::unique_ptr<base::Value>* result) override;
  Status GetFrameByFunction(const std::string& frame,
                            const std::string& function,
                            const base::Value::List& args,
                            std::string* out_frame) override;
  Status DispatchMouseEvents(const std::vector<MouseEvent>& events,
                             const std::string& frame,
                             bool async_dispatch_events) override;
  Status DispatchTouchEvent(const TouchEvent& event,
                            bool async_dispatch_events) override;
  Status DispatchTouchEvents(const std::vector<TouchEvent>& events,
                             bool async_dispatch_events) override;
  Status DispatchTouchEventWithMultiPoints(
      const std::vector<TouchEvent>& events,
      bool async_dispatch_events) override;
  Status DispatchKeyEvents(const std::vector<KeyEvent>& events,
                           bool async_dispatch_events) override;
  Status GetCookies(base::Value* cookies,
                    const std::string& current_page_url) override;
  Status DeleteCookie(const std::string& name,
                      const std::string& url,
                      const std::string& domain,
                      const std::string& path) override;
  Status AddCookie(const std::string& name,
                   const std::string& url,
                   const std::string& value,
                   const std::string& domain,
                   const std::string& path,
                   const std::string& same_site,
                   bool secure,
                   bool http_only,
                   double expiry) override;
  Status WaitForPendingNavigations(const std::string& frame_id,
                                   const Timeout& timeout,
                                   bool stop_load_on_timeout) override;
  Status IsPendingNavigation(const Timeout* timeout, bool* is_pending) override;
  Status WaitForPendingActivePage(const Timeout& timeout) override;
  Status IsNotPendingActivePage(const Timeout* timeout,
                                bool* is_not_pending) const override;
  Status GetActivePage(WebView** web_view) override;
  MobileEmulationOverrideManager* GetMobileEmulationOverrideManager()
      const override;
  Status OverrideGeolocation(const Geoposition& geoposition) override;
  Status OverrideNetworkConditions(
      const NetworkConditions& network_conditions) override;
  Status OverrideDownloadDirectoryIfNeeded(
      const std::string& download_directory) override;
  Status CaptureScreenshot(std::string* screenshot,
                           const base::Value::Dict& params) override;
  Status PrintToPDF(const base::Value::Dict& params, std::string* pdf) override;
  Status SetFileInputFiles(const std::string& frame,
                           const base::Value& element,
                           const std::vector<base::FilePath>& files,
                           const bool append) override;
  Status TakeHeapSnapshot(std::unique_ptr<base::Value>* snapshot) override;
  Status StartProfile() override;
  Status EndProfile(std::unique_ptr<base::Value>* profile_data) override;
  Status SynthesizeTapGesture(int x,
                              int y,
                              int tap_count,
                              bool is_long_press) override;
  Status SynthesizeScrollGesture(int x,
                                 int y,
                                 int xoffset,
                                 int yoffset) override;
  bool IsNonBlocking() const override;
  FrameTracker* GetFrameTracker() const override;
  Status GetFedCmTracker(FedCmTracker** out_tracker) override;
  std::unique_ptr<base::Value> GetCastSinks() override;
  std::unique_ptr<base::Value> GetCastIssueMessage() override;
  void SetFrame(const std::string& new_frame_id) override;
  Status GetBackendNodeIdByElement(const std::string& frame,
                                   const base::Value& element,
                                   int* node_id) override;
  bool IsDetached() const override;
  Status CallFunctionWithTimeout(const std::string& frame,
                                 const std::string& function,
                                 const base::Value::List& args,
                                 const base::TimeDelta& timeout,
                                 const CallFunctionOptions& options,
                                 std::unique_ptr<base::Value>* result) override;

  bool IsDialogOpen() const override;
  Status GetDialogMessage(std::string& message) const override;
  Status GetTypeOfDialog(std::string& type) const override;
  Status HandleDialog(bool accept,
                      const std::optional<std::string>& text) override;

  WebView* FindContainerForFrame(const std::string& frame_id) override;
  bool IsTab() const override;
  std::string GetTabId() override;
  PageTracker* GetPageTracker() const override;
  void SetupChildView(std::unique_ptr<StubWebView> child);

 private:
  std::string id_;
  std::string session_id_;
  std::unique_ptr<StubWebView> child_;
};

#endif  // CHROME_TEST_CHROMEDRIVER_CHROME_STUB_WEB_VIEW_H_
