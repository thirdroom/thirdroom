import { GroupCall, Invite, Room } from "@thirdroom/hydrogen-view-sdk";

import { useHydrogen } from "../../../hooks/useHydrogen";
import { getIdentifierColorNumber, getAvatarHttpUrl } from "../../../utils/avatar";
import { Avatar } from "../../../atoms/avatar/Avatar";
import { AvatarOutline } from "../../../atoms/avatar/AvatarOutline";
import { IconButton } from "../../../atoms/button/IconButton";
import { RoomTile } from "../../components/room-tile/RoomTile";
import { RoomTileTitle } from "../../components/room-tile/RoomTileTitle";
import { Category } from "../../components/category/Category";
import { CategoryHeader } from "../../components/category/CategoryHeader";
import { WorldTileMembers } from "./WorldTileMembers";
import { useRoomsOfType, RoomTypes } from "../../../hooks/useRoomsOfType";
import { useStore, OverlayWindow } from "../../../hooks/useStore";
import AddIC from "../../../../../res/ic/add.svg";
import AddUserIC from "../../../../../res/ic/add-user.svg";
import { JoinWorldDialog } from "../dialogs/JoinWorldDialog";
import { DropdownMenu } from "../../../atoms/menu/DropdownMenu";
import { DropdownMenuItem } from "../../../atoms/menu/DropdownMenuItem";
import { InviteDialog } from "../dialogs/InviteDialog";
import { useInvitesOfType } from "../../../hooks/useInvitesOfType";
import { AvatarBadgeWrapper } from "../../../atoms/avatar/AvatarBadgeWrapper";
import { NotificationBadge } from "../../../atoms/badge/NotificationBadge";

interface RoomListWorldProps {
  groupCalls: Map<string, GroupCall>;
}

export function RoomListWorld({ groupCalls }: RoomListWorldProps) {
  const { session, platform } = useHydrogen(true);

  const [worlds] = useRoomsOfType(session, RoomTypes.World);
  const [worldInvites] = useInvitesOfType(session, RoomTypes.World);

  const { selectedWorldId, selectWorld } = useStore((state) => state.overlayWorld);
  const { selectWindow } = useStore((state) => state.overlayWindow);

  const renderAvatar = (room: Room | Invite) => {
    const avatar = (
      <Avatar
        name={room.name || "Empty room"}
        size="xl"
        shape="circle"
        className="shrink-0"
        bgColor={`var(--usercolor${getIdentifierColorNumber(room.id)})`}
        imageSrc={getAvatarHttpUrl(room.avatarUrl || "", 70, platform, room.mediaRepository)}
      />
    );
    if (selectedWorldId === room.id) return <AvatarOutline>{avatar}</AvatarOutline>;
    return avatar;
  };

  return (
    <>
      <Category
        header={
          <CategoryHeader
            title="Worlds"
            options={
              <JoinWorldDialog
                renderTrigger={(openDialog) => (
                  <DropdownMenu
                    content={
                      <>
                        <DropdownMenuItem onSelect={() => selectWindow(OverlayWindow.CreateWorld)}>
                          Create World
                        </DropdownMenuItem>
                        <DropdownMenuItem onSelect={openDialog}>Join World</DropdownMenuItem>
                      </>
                    }
                  >
                    <IconButton size="sm" label="Create World" iconSrc={AddIC} />
                  </DropdownMenu>
                )}
              />
            }
          />
        }
      >
        {worldInvites.map((invite) => (
          <RoomTile
            key={invite.id}
            avatar={
              <AvatarBadgeWrapper badge={<NotificationBadge variant="secondary" content="Invite" />}>
                {renderAvatar(invite)}
              </AvatarBadgeWrapper>
            }
            content={<RoomTileTitle>{invite.name || "Empty room"}</RoomTileTitle>}
            onClick={() => selectWorld(invite.id)}
          />
        ))}
        {worlds.map((room) => {
          const groupCall = groupCalls.get(room.id);
          return (
            <RoomTile
              key={room.id}
              isActive={room.id === selectedWorldId}
              avatar={renderAvatar(room)}
              onClick={() => selectWorld(room.id)}
              content={
                <>
                  <RoomTileTitle>{room.name || "Empty room"}</RoomTileTitle>
                  {groupCall && <WorldTileMembers session={session} platform={platform} groupCall={groupCall} />}
                </>
              }
              options={
                <InviteDialog
                  key={room.id}
                  roomId={room.id}
                  renderTrigger={(openDialog) => (
                    <IconButton onClick={openDialog} iconSrc={AddUserIC} variant="surface-low" label="More options" />
                  )}
                />
              }
            />
          );
        })}
      </Category>
    </>
  );
}
