#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_unit.h"
#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_control_interfaces.h"

namespace sc2 {


//-------------------------------------------------------------------------------------------------
// ActionImp: an implementation of an ActionInterface.
//-------------------------------------------------------------------------------------------------

class ActionImp : public ActionInterface {
public:
    ProtoInterface& proto_;
    GameRequestPtr request_actions_;
    ControlInterface& control_;

    ActionImp(ProtoInterface& proto, ControlInterface& control);

    SC2APIProtocol::RequestAction* GetRequestAction();

    void UnitCommand(Tag unit_tag, AbilityID ability) override;
    void UnitCommand(Tag unit_tag, AbilityID ability, const Point2D& point) override;
    void UnitCommand(Tag unit_tag, AbilityID ability, Tag target_tag) override;

    void UnitCommand(const std::vector<Tag>& unit_tags, AbilityID ability) override;
    void UnitCommand(const std::vector<Tag>& unit_tags, AbilityID ability, const Point2D& point) override;
    void UnitCommand(const std::vector<Tag>& unit_tags, AbilityID ability, Tag target_tag) override;

    const std::vector<Tag>& Commands() const override;

    void SendActions() override;

    void ToggleAutocast(Tag unit_tag, AbilityID ability) override;
    void ToggleAutocast(const std::vector<Tag>& unit_tags, AbilityID ability) override;

    std::vector<Tag> commands_;
};

ActionImp::ActionImp(ProtoInterface& proto, ControlInterface& control) :
    proto_(proto),
    control_(control) {
}

SC2APIProtocol::RequestAction* ActionImp::GetRequestAction() {
    if (request_actions_ == nullptr) {
        request_actions_ = proto_.MakeRequest();
    }
    return request_actions_->mutable_action();
}

const std::vector<Tag>& ActionImp::Commands() const {
    return commands_;
}

void ActionImp::SendActions() {
    commands_.clear();

    if (request_actions_ == nullptr) {
        return;
    }

    if (!proto_.SendRequest(request_actions_)) {
        return;
    }

    SC2APIProtocol::RequestAction* request_action = GetRequestAction();
    if (request_action) {
        for (int i = 0, e = request_action->actions_size(); i < e; ++i) {
            const SC2APIProtocol::Action& action = request_action->actions(i);
            for (auto tag : action.action_raw().unit_command().unit_tags()) {
                commands_.push_back(tag);
            }
        }
    }

    request_actions_ = nullptr;
    control_.WaitForResponse();
}

void ActionImp::ToggleAutocast(Tag unit_tag, AbilityID ability) {
    std::vector<Tag> tags = { unit_tag };
    ToggleAutocast(tags, ability);
}

void ActionImp::ToggleAutocast(const std::vector<Tag>& unit_tags, AbilityID ability) {
    SC2APIProtocol::RequestAction* request_action = GetRequestAction();
    SC2APIProtocol::Action* action = request_action->add_actions();
    SC2APIProtocol::ActionRaw* action_raw = action->mutable_action_raw();
    SC2APIProtocol::ActionRawToggleAutocast* autocast = action_raw->mutable_toggle_autocast();
    for (const auto& u : unit_tags) {
        autocast->add_unit_tags(u);
    }
    autocast->set_ability_id(ability);
}

void ActionImp::UnitCommand(Tag unit_tag, AbilityID ability) {
    std::vector<Tag> tags = { unit_tag };
    UnitCommand(tags, ability);
}

void ActionImp::UnitCommand(Tag unit_tag, AbilityID ability, const Point2D& point) {
    std::vector<Tag> tags = { unit_tag };
    UnitCommand(tags, ability, point);
}

void ActionImp::UnitCommand(Tag unit_tag, AbilityID ability, Tag target_tag) {
    std::vector<Tag> tags = { unit_tag };
    UnitCommand(tags, ability, target_tag);
}

void ActionImp::UnitCommand(const std::vector<Tag>& unit_tags, AbilityID ability) {
    SC2APIProtocol::RequestAction* request_action = GetRequestAction();
    SC2APIProtocol::Action* action = request_action->add_actions();
    SC2APIProtocol::ActionRaw* action_raw = action->mutable_action_raw();
    SC2APIProtocol::ActionRawUnitCommand* unit_command = action_raw->mutable_unit_command();

    unit_command->set_ability_id(ability);

    for (auto tag : unit_tags) {
        unit_command->add_unit_tags(tag);
    }
}

void ActionImp::UnitCommand(const std::vector<Tag>& unit_tags, AbilityID ability, const Point2D& point) {
    SC2APIProtocol::RequestAction* request_action = GetRequestAction();
    SC2APIProtocol::Action* action = request_action->add_actions();
    SC2APIProtocol::ActionRaw* action_raw = action->mutable_action_raw();
    SC2APIProtocol::ActionRawUnitCommand* unit_command = action_raw->mutable_unit_command();

    unit_command->set_ability_id(ability);
    SC2APIProtocol::Point2D* target_point = unit_command->mutable_target_world_space_pos();
    target_point->set_x(point.x);
    target_point->set_y(point.y);

    for (auto tag : unit_tags) {
        unit_command->add_unit_tags(tag);
    }
}

void ActionImp::UnitCommand(const std::vector<Tag>& unit_tags, AbilityID ability, Tag target_tag) {
    SC2APIProtocol::RequestAction* request_action = GetRequestAction();
    SC2APIProtocol::Action* action = request_action->add_actions();
    SC2APIProtocol::ActionRaw* action_raw = action->mutable_action_raw();
    SC2APIProtocol::ActionRawUnitCommand* unit_command = action_raw->mutable_unit_command();

    unit_command->set_ability_id(ability);
    unit_command->set_target_unit_tag(target_tag);

    for (auto tag : unit_tags) {
        unit_command->add_unit_tags(tag);
    }
}


//-------------------------------------------------------------------------------------------------
// ActionFeatureLayerImp: an implementation of an ActionFeatureLayerInterface.
//-------------------------------------------------------------------------------------------------

class ActionFeatureLayerImp : public ActionFeatureLayerInterface {
public:
    ProtoInterface& proto_;
    ControlInterface& control_;
    GameRequestPtr request_actions_;

    ActionFeatureLayerImp(ProtoInterface& proto, ControlInterface& control);

    SC2APIProtocol::RequestAction* GetRequestAction();

    void UnitCommand(AbilityID ability) override;
    void UnitCommand(AbilityID ability, const Point2DI& point, bool minimap) override;
    void CameraMove(const Point2DI& center) override;
    void Select(const Point2DI& center, PointSelectionType selection_type) override;
    void Select(const Point2DI& p0, const Point2DI& p1, bool add_to_selection) override;

    void SendActions() override;
};

ActionFeatureLayerImp::ActionFeatureLayerImp(ProtoInterface& proto, ControlInterface& control) :
    proto_(proto),
    control_(control) {
}

SC2APIProtocol::RequestAction* ActionFeatureLayerImp::GetRequestAction() {
    if (request_actions_ == nullptr) {
        request_actions_ = proto_.MakeRequest();
    }
    return request_actions_->mutable_action();
}

void ActionFeatureLayerImp::SendActions() {
    if (request_actions_ == nullptr) {
        return;
    }

    if (!proto_.SendRequest(request_actions_)) {
        return;
    }

    request_actions_ = nullptr;
    control_.WaitForResponse();
}

void ActionFeatureLayerImp::UnitCommand(AbilityID ability) {
    SC2APIProtocol::RequestAction* request_action = GetRequestAction();
    SC2APIProtocol::Action* action = request_action->add_actions();
    SC2APIProtocol::ActionSpatial* action_feature_layer = action->mutable_action_feature_layer();
    SC2APIProtocol::ActionSpatialUnitCommand* unit_command = action_feature_layer->mutable_unit_command();
    unit_command->set_ability_id(ability);
}

void ActionFeatureLayerImp::UnitCommand(AbilityID ability, const Point2DI& point, bool minimap) {
    SC2APIProtocol::RequestAction* request_action = GetRequestAction();
    SC2APIProtocol::Action* action = request_action->add_actions();
    SC2APIProtocol::ActionSpatial* action_feature_layer = action->mutable_action_feature_layer();
    SC2APIProtocol::ActionSpatialUnitCommand* unit_command = action_feature_layer->mutable_unit_command();

    SC2APIProtocol::PointI* pt;
    if (minimap) {
        pt = unit_command->mutable_target_minimap_coord();
    }
    else {
        pt = unit_command->mutable_target_screen_coord();
    }
    pt->set_x(point.x);
    pt->set_y(point.y);
    unit_command->set_ability_id(ability);
}

void ActionFeatureLayerImp::CameraMove(const Point2DI& center) {
    SC2APIProtocol::RequestAction* request_action = GetRequestAction();
    SC2APIProtocol::Action* action = request_action->add_actions();
    SC2APIProtocol::ActionSpatial* action_feature_layer = action->mutable_action_feature_layer();
    SC2APIProtocol::ActionSpatialCameraMove* camera_move = action_feature_layer->mutable_camera_move();

    SC2APIProtocol::PointI* center_proto = camera_move->mutable_center_minimap();
    center_proto->set_x(center.x);
    center_proto->set_y(center.y);
}

void ActionFeatureLayerImp::Select(const Point2DI& center, PointSelectionType selection_type) {
    SC2APIProtocol::RequestAction* request_action = GetRequestAction();
    SC2APIProtocol::Action* action = request_action->add_actions();
    SC2APIProtocol::ActionSpatial* action_feature_layer = action->mutable_action_feature_layer();
    SC2APIProtocol::ActionSpatialUnitSelectionPoint* select_pt = action_feature_layer->mutable_unit_selection_point();

    SC2APIProtocol::PointI* center_proto = select_pt->mutable_selection_screen_coord();
    center_proto->set_x(center.x);
    center_proto->set_y(center.y);
    select_pt->set_type(static_cast<SC2APIProtocol::ActionSpatialUnitSelectionPoint_Type>(selection_type));
}

void ActionFeatureLayerImp::Select(const Point2DI& p0, const Point2DI& p1, bool /*add_to_selection*/) {
    SC2APIProtocol::RequestAction* request_action = GetRequestAction();
    SC2APIProtocol::Action* action = request_action->add_actions();
    SC2APIProtocol::ActionSpatial* action_feature_layer = action->mutable_action_feature_layer();
    SC2APIProtocol::ActionSpatialUnitSelectionRect* select_rect = action_feature_layer->mutable_unit_selection_rect();
    SC2APIProtocol::RectangleI* selection_screen_coord = select_rect->add_selection_screen_coord();
    SC2APIProtocol::PointI* selection_p0 = selection_screen_coord->mutable_p0();
    selection_p0->set_x(p0.x);
    selection_p0->set_y(p0.y);
    SC2APIProtocol::PointI* selection_p1 = selection_screen_coord->mutable_p1();
    selection_p1->set_x(p1.x);
    selection_p1->set_y(p1.y);
}

//-------------------------------------------------------------------------------------------------
// AgentControlImp: an implementation of AgentControlInterface.
//-------------------------------------------------------------------------------------------------

class AgentControlImp : public AgentControlInterface {
public:
    ControlInterface* control_interface_;
    std::unique_ptr<ActionImp> actions_;
    std::unique_ptr<ActionFeatureLayerImp> actions_feature_layer_;
    Agent* agent_;

    AgentControlImp(Agent* agent, ControlInterface* control_interface);
    ~AgentControlImp() = default;

    bool Restart() override;
};

AgentControlImp::AgentControlImp(Agent* agent, ControlInterface* control_interface) :
    control_interface_(control_interface),
    actions_(nullptr),
    agent_(agent) {
    actions_ = std::make_unique<ActionImp>(control_interface_->Proto(), *control_interface);
    actions_feature_layer_ = std::make_unique<ActionFeatureLayerImp>(control_interface_->Proto(), *control_interface);
}

bool AgentControlImp::Restart() {
    GameRequestPtr request = control_interface_->Proto().MakeRequest();
    request->mutable_restart_game();
    if (!control_interface_->Proto().SendRequest(request)) {
        return false;
    }

    GameResponsePtr response = control_interface_->WaitForResponse();
    if (!response.get()) {
        assert(0);
        return false;
    }
    if (!response->has_restart_game()) {
        assert(0);
        return false;
    }
    const SC2APIProtocol::ResponseRestartGame& response_restart_game = response->restart_game();
    if (response_restart_game.has_error()) {
        // TODO: Output the error.
        assert(0);
        return false;
    }

    agent_->OnGameStart();

    return control_interface_->IsInGame();
}


//-------------------------------------------------------------------------------------------------
// Agent implementation.
//-------------------------------------------------------------------------------------------------

Agent::Agent() :
    agent_control_imp_(nullptr) {
    agent_control_imp_ = new AgentControlImp(this, Control());
}

Agent::~Agent() {
    delete agent_control_imp_;
}

ActionInterface* Agent::Actions() {
    return agent_control_imp_->actions_.get();
}

ActionFeatureLayerInterface* Agent::ActionsFeatureLayer() {
    return agent_control_imp_->actions_feature_layer_.get();
}

AgentControlInterface* Agent::AgentControl() {
    return agent_control_imp_;
}

}
