import { Platform, Session } from "@thirdroom/hydrogen-view-sdk";

import { Avatar } from "../../../atoms/avatar/Avatar";
import { Button } from "../../../atoms/button/Button";
import { RoomPreviewCard } from "../../components/room-preview-card/RoomPreviewCard";
import MoreHorizontalIC from "../../../../../res/ic/more-horizontal.svg";
import { RoomSummaryProvider } from "../../components/RoomSummaryProvider";
import { getAvatarHttpUrl, getIdentifierColorNumber } from "../../../utils/avatar";
import { Dots } from "../../../atoms/loading/Dots";
import { IconButton } from "../../../atoms/button/IconButton";
import { DropdownMenu } from "../../../atoms/menu/DropdownMenu";
import { DropdownMenuItem } from "../../../atoms/menu/DropdownMenuItem";
import { SidebarTabs, useStore } from "../../../hooks/useStore";
import { JoinRoomProvider } from "../../components/JoinRoomProvider";
import { RepositoryEvents } from "./DiscoverView";

export function FeaturedWorldCard({
  session,
  platform,
  repoRoomId,
  roomId,
  canEdit,
}: {
  session: Session;
  platform: Platform;
  repoRoomId: string;
  roomId: string;
  canEdit: boolean;
}) {
  const handleRemoveFeatured = () => {
    session.hsApi.sendState(repoRoomId, RepositoryEvents.FeaturedWorlds, roomId, {});
  };
  const handleViewWorld = () => {
    const state = useStore.getState();
    state.overlayWorld.selectWorld(roomId);
    state.overlaySidebar.selectSidebarTab(SidebarTabs.Home);
    state.overlayWindow.closeWindow();
  };

  return (
    <RoomSummaryProvider roomIdOrAlias={roomId} fallback={() => <RoomPreviewCard />}>
      {(summaryData) => (
        <RoomPreviewCard
          avatar={
            <Avatar
              imageSrc={
                summaryData.avatarUrl && getAvatarHttpUrl(summaryData.avatarUrl, 60, platform, session.mediaRepository)
              }
              shape="circle"
              size="lg"
              bgColor={`var(--usercolor${getIdentifierColorNumber(summaryData.roomId)})`}
              name={summaryData.name}
            />
          }
          name={summaryData.name}
          desc={summaryData.topic}
          memberCount={summaryData.memberCount}
          options={
            <div className="flex items-center gap-xs">
              {canEdit && (
                <DropdownMenu
                  content={
                    <DropdownMenuItem onSelect={handleRemoveFeatured} variant="danger">
                      Remove Featured
                    </DropdownMenuItem>
                  }
                >
                  <IconButton label="Options" iconSrc={MoreHorizontalIC} />
                </DropdownMenu>
              )}
              <JoinRoomProvider session={session}>
                {(join, isJoined, loading, error) =>
                  isJoined ? (
                    <Button variant="secondary" fill="outline" size="sm" onClick={handleViewWorld}>
                      View
                    </Button>
                  ) : (
                    <Button
                      disabled={loading}
                      variant="secondary"
                      size="sm"
                      onClick={() => join(summaryData.alias ?? roomId)}
                    >
                      {loading && <Dots size="sm" color="on-secondary" />}
                      {loading ? "Joining" : "Join"}
                    </Button>
                  )
                }
              </JoinRoomProvider>
            </div>
          }
        />
      )}
    </RoomSummaryProvider>
  );
}
