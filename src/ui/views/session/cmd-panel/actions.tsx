import { GroupCall, Room } from "@thirdroom/hydrogen-view-sdk";
import { useSetAtom } from "jotai";
import { useRegisterActions } from "kbar";
import { useCallback, useState } from "react";
import { useNavigate } from "react-router-dom";

import { Thread } from "../../../../engine/module/module.common";
import { NametagsEnableMessage, NametagsEnableMessageType } from "../../../../plugins/nametags/nametags.common";
import { IconButton } from "../../../atoms/button/IconButton";
import { Dialog } from "../../../atoms/dialog/Dialog";
import { Header } from "../../../atoms/header/Header";
import { HeaderTitle } from "../../../atoms/header/HeaderTitle";
import { Scroll } from "../../../atoms/scroll/Scroll";
import { useHydrogen } from "../../../hooks/useHydrogen";
import { useMainThreadContext } from "../../../hooks/useMainThread";
import { OverlayWindow, overlayWindowAtom } from "../../../state/overlayWindow";
import { manageMuteRequest, MicExceptionDialog, useMuteButton } from "../../components/MuteButtonProvider";
import { MemberListDialog } from "../dialogs/MemberListDialog";
import { ShortcutUI } from "../world/ShortcutUI";
import CrossIC from "../../../../../res/ic/cross.svg";
import { togglePhysicsDebug } from "../../../../plugins/thirdroom/thirdroom.main";

enum ActionSection {
  Global = "Global",
  World = "World",
}

export const useUserProfileAction = () => {
  const setOverlayWindow = useSetAtom(overlayWindowAtom);

  useRegisterActions(
    [
      {
        id: "user-profile",
        name: "User Profile",
        shortcut: undefined,
        keywords: "user profile",
        section: ActionSection.Global,
        icon: undefined,
        subtitle: undefined,
        perform: () => setOverlayWindow({ type: OverlayWindow.UserProfile }),
        parent: undefined,
      },
    ],
    []
  );
};

export const useAccountManagementAction = () => {
  const { session } = useHydrogen(true);
  const { accountManagementUrl } = session.sessionInfo;

  useRegisterActions(
    [
      {
        id: "manage-account",
        name: "Manage Account",
        shortcut: undefined,
        keywords: "account",
        section: ActionSection.Global,
        icon: undefined,
        subtitle: undefined,
        perform: () => window.open(accountManagementUrl),
        parent: undefined,
      },
    ],
    [accountManagementUrl]
  );
};

export const useTechPreviewAction = () => {
  const navigate = useNavigate();
  useRegisterActions(
    [
      {
        id: "tech-preview",
        name: "Tech Preview",
        shortcut: undefined,
        keywords: "preview",
        section: ActionSection.Global,
        icon: undefined,
        subtitle: undefined,
        perform: () => navigate("/preview"),
        parent: undefined,
      },
    ],
    []
  );
};

export const MuteButtonAction = ({
  activeCall,
  showToast,
}: {
  activeCall?: GroupCall;
  showToast?: (text: string) => void;
}) => {
  const { mute, requestStream, handleMute, micException, setMicException } = useMuteButton(activeCall);

  useRegisterActions(
    [
      {
        id: "toggle-mute",
        name: "Toggle Mute",
        shortcut: ["M"],
        keywords: "mute",
        section: ActionSection.World,
        icon: undefined,
        subtitle: undefined,
        perform: () => {
          showToast?.(!mute ? "Microphone Muted" : "Microphone Unmuted");
          handleMute(async () => manageMuteRequest(requestStream, setMicException));
        },
        parent: undefined,
      },
    ],
    [requestStream, showToast, mute]
  );

  return <MicExceptionDialog micException={micException} setMicException={setMicException} />;
};

export const useToggleNamesAction = (
  showNames: boolean,
  setShowNames: (value: boolean) => void,
  showToast?: (text: string) => void
) => {
  const mainThread = useMainThreadContext();

  const toggleShowNames = useCallback(() => {
    const enabled = !showNames;
    setShowNames(enabled);
    mainThread.sendMessage<NametagsEnableMessageType>(Thread.Game, { type: NametagsEnableMessage, enabled });
    showToast?.(enabled ? "Show Names" : "Hide Names");
  }, [mainThread, showNames, showToast, setShowNames]);

  useRegisterActions(
    [
      {
        id: "toggle-names",
        name: "Toggle Names",
        shortcut: ["N"],
        keywords: "preview",
        section: ActionSection.World,
        icon: undefined,
        subtitle: undefined,
        perform: () => toggleShowNames(),
        parent: undefined,
      },
    ],
    [toggleShowNames]
  );
};

export const MembersDialogAction = ({ world }: { world: Room }) => {
  const [showActiveMembers, setShowActiveMembers] = useState<boolean>(false);

  useRegisterActions(
    [
      {
        id: "members-dialog",
        name: "Members",
        shortcut: ["P"],
        keywords: "members",
        section: ActionSection.World,
        icon: undefined,
        subtitle: undefined,
        perform: () => setShowActiveMembers((state) => !state),
        parent: undefined,
      },
    ],
    []
  );

  return (
    <Dialog open={showActiveMembers} onOpenChange={setShowActiveMembers}>
      <MemberListDialog room={world} requestClose={() => setShowActiveMembers(false)} />
    </Dialog>
  );
};

export const ShortcutDialogAction = () => {
  const [shortcutUI, setShortcutUI] = useState(false);

  useRegisterActions(
    [
      {
        id: "shortcuts-dialog",
        name: "Shortcuts",
        shortcut: ["/"],
        keywords: "shortcuts",
        section: ActionSection.World,
        icon: undefined,
        subtitle: undefined,
        perform: () => setShortcutUI((state) => !state),
        parent: undefined,
      },
    ],
    []
  );

  return (
    <Dialog open={shortcutUI} onOpenChange={setShortcutUI}>
      <Header
        left={<HeaderTitle size="lg">Controls</HeaderTitle>}
        right={<IconButton iconSrc={CrossIC} onClick={() => setShortcutUI((state) => !state)} label="Close" />}
      />
      <div className="flex" style={{ height: "600px" }}>
        <Scroll type="hover">
          <ShortcutUI />
        </Scroll>
      </div>
    </Dialog>
  );
};

export const useToggleEditorAction = (setOpen: (value: boolean | ((value: boolean) => boolean)) => void) => {
  useRegisterActions(
    [
      {
        id: "editor",
        name: "Toggle Editor",
        shortcut: ["`"],
        keywords: "editor",
        section: ActionSection.World,
        icon: undefined,
        subtitle: undefined,
        perform: () => setOpen((state) => !state),
        parent: undefined,
      },
    ],
    []
  );
};

export const useTogglePhysicsDebugAction = () => {
  const mainThread = useMainThreadContext();

  useRegisterActions(
    [
      {
        id: "physics-debug",
        name: "Toggle Physics Debug",
        keywords: "physics debug",
        section: ActionSection.World,
        icon: undefined,
        subtitle: undefined,
        perform: () => {
          togglePhysicsDebug(mainThread);
        },
        parent: undefined,
      },
    ],
    [mainThread]
  );
};

export const useToggleStatsAction = (setOpen: (value: boolean | ((value: boolean) => boolean)) => void) => {
  useRegisterActions(
    [
      {
        id: "stats",
        name: "Toggle Stats",
        shortcut: ["Shift+Control+S"],
        keywords: "stats",
        section: ActionSection.World,
        icon: undefined,
        subtitle: undefined,
        perform: () => setOpen((state) => !state),
        parent: undefined,
      },
    ],
    []
  );
};

export const EnterWebXRAction = ({ enter }: { enter: () => void }) => {
  useRegisterActions(
    [
      {
        id: "webxr",
        name: "Enter WebXR",
        shortcut: ["Alt+X"],
        keywords: "webxr",
        section: ActionSection.World,
        icon: undefined,
        subtitle: undefined,
        perform: () => enter(),
        parent: undefined,
      },
    ],
    []
  );

  return null;
};
