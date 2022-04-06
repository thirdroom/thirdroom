import { useEffect, useRef } from "react";
import { TimelineViewModel } from "hydrogen-view-sdk";

import { Text } from "../../../atoms/text/Text";
import { Scroll } from "../../../atoms/scroll/Scroll";
import { TextTile } from "./tiles/TextTile";
import "./TimelineView.css";

interface ITimelineView {
  roomId: string;
  vm: typeof TimelineViewModel;
}

export function TimelineView({ roomId, vm }: ITimelineView) {
  window.tvm = vm;
  window.tiles = vm.tiles;
  const { tiles } = vm;
  const timelineScrollRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    const onReset = () => {};
    const onAdd = () => {};
    const onUpdate = () => {};
    const onRemove = () => {};
    const onMove = () => {};

    return tiles.subscribe({
      onReset,
      onAdd,
      onUpdate,
      onRemove,
      onMove,
    });
  }, [roomId, tiles]);

  // useLayoutEffect(() => {
  //   const tScroll = timelineScrollRef.current;
  //   if (tScroll !== null) {
  //     const { clientHeight, scrollHeight } = tScroll;
  //     tScroll.scrollTop = scrollHeight - clientHeight;
  //     console.log(tScroll.scrollTop, clientHeight, scrollHeight);
  //   }
  // }, []);

  const renderTimeline = () => {
    const reactTiles = [];
    for (const tile of tiles) {
      const sender = tile.displayName;
      const plainBody = tile._getPlainBody?.();
      if (plainBody) reactTiles.push(<TextTile key={tile.id.eventIndex} sender={sender} body={plainBody} />);
    }
    return reactTiles;
  };

  return (
    <div className="TimelineView grow">
      <Scroll forwardRef={timelineScrollRef} visibility="invisible">
        <div className="TimelineView__content flex flex-column justify-end items-start">
          {tiles.hasSubscriptions && renderTimeline()}
          {tiles.hasSubscriptions === false && <Text>loading...</Text>}
        </div>
      </Scroll>
    </div>
  );
}
