/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is Mozilla Foundation
 * Portions created by the Initial Developer are Copyright (C) 2011
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Mounir Lamouri <mounir.lamouri@mozilla.com> (Original Author)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either of the GNU General Public License Version 2 or later (the "GPL"),
 * or the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef mozilla_dom_sms_SmsParent_h
#define mozilla_dom_sms_SmsParent_h

#include "mozilla/dom/sms/PSmsParent.h"
#include "nsIObserver.h"

namespace mozilla {
namespace dom {
namespace sms {

class SmsParent : public PSmsParent
                , public nsIObserver
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIOBSERVER

  static void GetAll(nsTArray<SmsParent*>& aArray);

  SmsParent();

  NS_OVERRIDE virtual bool RecvHasSupport(bool* aHasSupport);
  NS_OVERRIDE virtual bool RecvGetNumberOfMessagesForText(const nsString& aText, PRUint16* aResult);
  NS_OVERRIDE virtual bool RecvSendMessage(const nsString& aNumber, const nsString& aMessage, const PRInt32& aRequestId, const PRUint64& aProcessId);
  NS_OVERRIDE virtual bool RecvSaveReceivedMessage(const nsString& aSender, const nsString& aBody, const PRUint64& aDate, PRInt32* aId);
  NS_OVERRIDE virtual bool RecvSaveSentMessage(const nsString& aRecipient, const nsString& aBody, const PRUint64& aDate, PRInt32* aId);
  NS_OVERRIDE virtual bool RecvGetMessage(const PRInt32& aMessageId, const PRInt32& aRequestId, const PRUint64& aProcessId);
  NS_OVERRIDE virtual bool RecvDeleteMessage(const PRInt32& aMessageId, const PRInt32& aRequestId, const PRUint64& aProcessId);
  NS_OVERRIDE virtual bool RecvCreateMessageList(const SmsFilterData& aFilter, const bool& aReverse, const PRInt32& aRequestId, const PRUint64& aProcessId);
  NS_OVERRIDE virtual bool RecvGetNextMessageInList(const PRInt32& aListId, const PRInt32& aRequestId, const PRUint64& aProcessId);
  NS_OVERRIDE virtual bool RecvClearMessageList(const PRInt32& aListId);

protected:
  virtual void ActorDestroy(ActorDestroyReason why);

private:
  static nsTArray<SmsParent*>* gSmsParents;
};

} // namespace sms
} // namespace dom
} // namespace mozilla

#endif // mozilla_dom_sms_SmsParent_h
