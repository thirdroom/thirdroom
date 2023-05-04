import { Icon } from "../../../atoms/icon/Icon";
import { Text } from "../../../atoms/text/Text";
import { ToolbarButton, ToolbarButtonGroup, ToolbarButtonDivider } from "./ToolbarButton";
import ExploreIC from "../../../../../res/ic/explore.svg";
import ChevronBottomIC from "../../../../../res/ic/chevron-bottom.svg";
import MenuIC from "../../../../../res/ic/menu.svg";
import { Toolbar, ToolbarItemGroup } from "./Toolbar";

export const title = "Toolbar";

export default function ToolbarStories() {
  return (
    <div style={{ padding: "var(--sp-md)", backgroundColor: "var(--bg-surface)" }}>
      <Toolbar>
        <ToolbarItemGroup>
          <ToolbarButton>
            <Icon size="sm" src={MenuIC} />
          </ToolbarButton>

          <ToolbarButton outlined>
            <Text variant="b3" weight="semi-bold">
              Toolbar Button
            </Text>
          </ToolbarButton>

          <ToolbarButton before={<Icon size="sm" src={ExploreIC} />} outlined>
            <Text variant="b3" weight="semi-bold">
              Toolbar Button
            </Text>
          </ToolbarButton>

          <ToolbarButton outlined>
            <Icon size="sm" src={ExploreIC} />
          </ToolbarButton>

          <ToolbarButtonGroup>
            <ToolbarButton active={true}>
              <Icon color="primary" size="sm" src={ExploreIC} />
            </ToolbarButton>
            <ToolbarButtonDivider />
            <ToolbarButton>
              <Icon size="sm" src={ChevronBottomIC} />
            </ToolbarButton>
          </ToolbarButtonGroup>
        </ToolbarItemGroup>
      </Toolbar>
    </div>
  );
}
