/*
 * Copyright 2006 Milan Digital Audio LLC
 * Copyright 2009-2025 GrandOrgue contributors (see AUTHORS)
 * License GPL-2.0 or later
 * (https://www.gnu.org/licenses/old-licenses/gpl-2.0.html).
 */

#include "GOMidiEventDialog.h"

#include <wx/bookctrl.h>

#include "config/GOConfig.h"
#include "midi/dialog-creator/GOMidiDialogListener.h"
#include "midi/objects/GOMidiObject.h"

#include "GOMidiEventKeyTab.h"
#include "GOMidiEventRecvTab.h"
#include "GOMidiEventSendTab.h"

GOMidiEventDialog::GOMidiEventDialog(
  GODocumentBase *doc,
  wxWindow *parent,
  const wxString &title,
  GOConfig &settings,
  const wxString &dialogSelector,
  GOMidiObject *pMidiObject,
  GOMidiReceiver *event,
  GOMidiSender *sender,
  GOMidiShortcutReceiver *key,
  GOMidiSender *division,
  GOMidiDialogListener *pDialogListener)
  : GOTabbedDialog(
    parent, "MidiEvent", title, settings.m_DialogSizes, dialogSelector),
    GOView(doc, this),
    p_DialogListener(pDialogListener),
    m_recvPage(NULL),
    m_sendPage(NULL),
    m_sendDivisionPage(NULL),
    m_keyPage(NULL) {
  wxBookCtrlBase *notebook = GetBookCtrl();

  if (event) {
    m_recvPage = new GOMidiEventRecvTab(notebook, event, settings);
    m_recvPage->SetModificationListener(this);
    AddTab(m_recvPage, "Receive", _("Receive"));
  }
  if (sender) {
    m_sendPage
      = new GOMidiEventSendTab(this, _("Send"), sender, m_recvPage, settings);
    m_sendPage->SetModificationListener(this);
    AddTab(m_sendPage);
  }
  if (key) {
    m_keyPage = new GOMidiEventKeyTab(notebook, key);
    AddTab(m_keyPage, "Shortcut", _("Shortcut"));
  }
  if (division) {
    m_sendDivisionPage = new GOMidiEventSendTab(
      this, _("Send Division Output"), division, m_recvPage, settings);
    AddTab(m_sendDivisionPage);
  }

  LayoutDialog();
}

GOMidiEventDialog::GOMidiEventDialog(
  GODocumentBase *doc,
  wxWindow *parent,
  const wxString &title,
  GOConfig &settings,
  const wxString &dialogSelector,
  GOMidiObject &midiObject,
  GOMidiDialogListener *pDialogListener)
  : GOMidiEventDialog(
    doc,
    parent,
    title,
    settings,
    dialogSelector,
    &midiObject,
    !midiObject.IsReadOnly() ? midiObject.GetMidiReceiver() : nullptr,
    midiObject.GetMidiSender(),
    !midiObject.IsReadOnly() ? midiObject.GetMidiShortcutReceiver() : nullptr,
    midiObject.GetDivisionSender(),
    pDialogListener) {}

GOMidiEventDialog::GOMidiEventDialog(
  wxWindow *parent,
  const wxString &title,
  GOConfig &settings,
  const wxString &dialogSelector,
  GOMidiReceiver *pReceiver)
  : GOMidiEventDialog(
    nullptr,
    parent,
    title,
    settings,
    dialogSelector,
    nullptr,
    pReceiver,
    nullptr,
    nullptr,
    nullptr,
    nullptr) {}

void GOMidiEventDialog::RegisterMIDIListener(GOMidi *midi) {
  if (m_recvPage)
    m_recvPage->RegisterMIDIListener(midi);
}

bool GOMidiEventDialog::TransferDataFromWindow() {
  bool res = GOTabbedDialog::TransferDataFromWindow();

  if (res && p_DialogListener)
    p_DialogListener->OnSettingsApplied();
  return res;
}
