#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "UObject/ObjectMacros.h"

#include "FlowTypes.h"
#include "FlowGraphNode.generated.h"

class UEdGraphPin;
class UEdGraphSchema;

class UFlowNode;

USTRUCT()
struct FFlowBreakpoint
{
	GENERATED_USTRUCT_BODY()
	
public:
	UPROPERTY()
	uint32 bHasBreakpoint : 1;

	uint32 bBreakpointEnabled : 1;
	uint32 bBreakpointHit : 1;

	FFlowBreakpoint() {};

	void AddBreakpoint();
	void RemoveBreakpoint();
	bool HasBreakpoint() const;

	void EnableBreakpoint();
	bool CanEnableBreakpoint() const;

	void DisableBreakpoint();
	bool IsBreakpointEnabled() const;

	void ToggleBreakpoint();
};

/**
 * Graph representation of the Flow Node
 */
UCLASS()
class FLOWEDITOR_API UFlowGraphNode : public UEdGraphNode
{
	GENERATED_UCLASS_BODY()
	
//////////////////////////////////////////////////////////////////////////
// Flow node
	
private:
	UPROPERTY(Instanced)
	UFlowNode* FlowNode;

public:
	void SetFlowNode(UFlowNode* InFlowNode);
	UFlowNode* GetFlowNode() const;

	virtual void PostLoad() override;
	virtual void PostDuplicate(bool bDuplicateForPIE) override;
	virtual void PostEditImport() override;
	virtual void PrepareForCopying() override;
	void PostCopyNode();

private:
	/** Make sure the FlowNode is owned by the FlowAsset */
	void ResetFlowNodeOwner();

//////////////////////////////////////////////////////////////////////////
// Graph node

public:
	UPROPERTY()
	FFlowBreakpoint NodeBreakpoint;

	// UEdGraphNode
	virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const override;
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
	// --

	/**
	 * Handles inserting the node between the FromPin and what the FromPin was original connected to
	 *
	 * @param FromPin			The pin this node is being spawned from
	 * @param NewLinkPin		The new pin the FromPin will connect to
	 * @param OutNodeList		Any nodes that are modified will get added to this list for notification purposes
	 */
	void InsertNewNode(UEdGraphPin* FromPin, UEdGraphPin* NewLinkPin, TSet<UEdGraphNode*>& OutNodeList);

	// UEdGraphNode
	virtual void ReconstructNode() override;
	virtual void AllocateDefaultPins() override;

	virtual void GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;

	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual FText GetTooltipText() const override;
	// --

//////////////////////////////////////////////////////////////////////////
// Utils

public:
	// short summary of node's content
	FString GetNodeDescription() const;
	
	// get flow node for the inspected asset instance
	UFlowNode* GetInspectedNodeInstance() const;

	// used for highlighting active nodes of the inspected asset instance
	EFlowActivationState GetActivationState() const;

	// information displayed while node is active
	FString GetStatusString() const;

	// check this to display information while node is preloaded
	bool IsContentPreloaded() const;

	UObject* GetAssetToOpen() const;
	bool CanFocusViewport() const;

//////////////////////////////////////////////////////////////////////////
// Pins

public:
	TArray<UEdGraphPin*> InputPins;
	TArray<UEdGraphPin*> OutputPins;

	UPROPERTY()
	TMap<int32, FFlowBreakpoint> InputBreakpoints;

	UPROPERTY()
	TMap<int32, FFlowBreakpoint> OutputBreakpoints;

	void CreateInputPin(const FName& PinName);
	void CreateOutputPin(const FName PinName);

	bool CanUserAddInput() const;
	bool CanUserAddOutput() const;

	bool CanUserRemoveInput(const UEdGraphPin* Pin) const;
	bool CanUserRemoveOutput(const UEdGraphPin* Pin) const;

	void AddUserInput();
	void AddUserOutput();

	void RemoveUserPin(UEdGraphPin* Pin);

//////////////////////////////////////////////////////////////////////////
// Breakpoints

public:
	void OnInputTriggered(const int32 Index);
	void OnOutputTriggered(const int32 Index);

private:
	void TryPausingSession(bool bPauseSession);

	void OnResumePIE(const bool bIsSimulating);
	void OnEndPIE(const bool bIsSimulating);
	void ResetBreakpoints();
};