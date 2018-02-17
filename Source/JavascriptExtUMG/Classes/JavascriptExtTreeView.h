#pragma once

#include "STreeView.h"
#include "TableViewBase.h"
#include "JavascriptExtTreeView.generated.h"

class UJavascriptContext;

USTRUCT(BlueprintType)
struct FJavascriptExtColumn
{
    GENERATED_BODY()
    
    UPROPERTY()
    FName Id;
    
    UPROPERTY()
    float Width;
    
    UPROPERTY(Transient)
    UWidget* Widget;
};

/**
* Allows thousands of items to be displayed in a list.  Generates widgets dynamically for each item.
*/
UCLASS(Experimental)
class JAVASCRIPTEXTUMG_API UJavascriptExtTreeView : public UTableViewBase
{
	GENERATED_UCLASS_BODY()

public:	
	/** Delegate for constructing a UWidget based on a UObject */
	DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnGetChildrenUObject, UObject*, Item, UJavascriptExtTreeView*, Instance);

	/** Delegate for constructing a UWidget based on a UObject */
	DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnExpansionChanged, UObject*, Item, bool, bExpanded, UJavascriptExtTreeView*, Instance);

	/** Delegate for constructing a UWidget based on a UObject */
	DECLARE_DYNAMIC_DELEGATE_RetVal_ThreeParams(UWidget*, FOnGenerateRow, UObject*, Object, FName, Id, UJavascriptExtTreeView*, Instance);

	/** Delegate for constructing a UWidget based on a UObject */
	DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(UWidget*, FOnContextMenuOpening, UJavascriptExtTreeView*, Instance);

	/** Delegate for constructing a UWidget based on a UObject */
	DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnColumnRefreshed, FName, Id, float, NewSize, UJavascriptExtTreeView*, Instance);

	/** Called when a widget needs to be generated */
	UPROPERTY(EditAnywhere, Category = Events, meta = (IsBindableEvent = "True"))
	FOnGenerateRow OnGenerateRowEvent;
	
	UPROPERTY(EditAnywhere, Category = Events, meta = (IsBindableEvent = "True"))
	FOnExpansionChanged OnExpansionChanged;

	UPROPERTY(EditAnywhere, Category = Events, meta = (IsBindableEvent = "True"))
	FOnContextMenuOpening OnContextMenuOpening;

	UPROPERTY(EditAnywhere, Category = Events, meta = (IsBindableEvent = "True"))
	FOnColumnRefreshed OnColumnRefreshed;

	/** Called when a widget needs to be generated */
	UPROPERTY(EditAnywhere, Category = Events, meta = (IsBindableEvent = "True"))
	FOnGetChildrenUObject OnGetChildren;	

	UPROPERTY(BlueprintReadWrite, Category = "Javascript")
	UJavascriptContext* JavascriptContext;

	/** The list of items to generate widgets for */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Content)
	TArray<UObject*> Items;

	UPROPERTY(EditAnywhere, BlueprintInternalUseOnly, Category = "Javascript")
	FHeaderRowStyle HeaderRowStyle;

	UPROPERTY(EditAnywhere, BlueprintInternalUseOnly, Category = "Javascript")
	FTableRowStyle TableRowStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Javascript")
	FScrollBarStyle ScrollBarStyle;

	/** The selection method for the list */
	UPROPERTY(EditAnywhere, Category = Content)
	TEnumAsByte<ESelectionMode::Type> SelectionMode;

	UPROPERTY(BlueprintReadWrite, Transient, Category = "Javascript")
	mutable TArray<UObject*> Children;

	UPROPERTY(BlueprintReadWrite, Transient, Category = "Javascript")
	TArray<FJavascriptExtColumn> Columns;

	/** Refreshes the list */
	UFUNCTION(BlueprintCallable, Category = "Behavior")
	void RequestTreeRefresh();

	/** Event fired when a tutorial stage ends */
	UFUNCTION(BlueprintImplementableEvent, Category = "Javascript")
	void OnDoubleClick(UObject* Object);

	/** Event fired when a tutorial stage ends */
	UFUNCTION(BlueprintImplementableEvent, Category = "Javascript")
	void OnSelectionChanged(UObject* Object, ESelectInfo::Type Type);

	UFUNCTION(BlueprintCallable, Category = "Javascript")
	void GetSelectedItems(TArray<UObject*>& OutItems);

	UFUNCTION(BlueprintCallable, Category = "Javascript")
	void SetSelection(UObject* SoleSelectedItem);

	UFUNCTION(BlueprintCallable, Category = "Javascript")
	void SetItemExpansion(UObject* InItem, bool InShouldExpandItem);

	UFUNCTION(BlueprintCallable, Category = "Javascript")
	void SetSingleExpandedItem(UObject* InItem);

	UFUNCTION(BlueprintCallable, Category = "Javascript")
	void SetColumnSize(FName ColumnId, float InSize);

	UFUNCTION(BlueprintCallable, Category = "Javascript")
	bool IsItemExpanded(UObject* InItem);

	TSharedRef<ITableRow> HandleOnGenerateRow(UObject* Item, const TSharedRef< STableViewBase >& OwnerTable);

	void HandleOnGetChildren(UObject* Item, TArray<UObject*>& OutChildItems);
	void HandleOnExpansionChanged(UObject* Item, bool bExpanded);
	void HandleOnColumnRefreshed();

	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void SetVisibility(ESlateVisibility InVisibility) override;
	// End of UWidget interface

	// UObject interface
	virtual void ProcessEvent(UFunction* Function, void* Parms) override;
	// End of UObject interface

	//~ Begin UVisual Interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	//~ End UVisual Interface

	TSharedPtr< STreeView<UObject*> > MyTreeView;

	TSharedPtr<SHeaderRow> GetHeaderRowWidget();

	UPROPERTY(Transient)
	TArray<UWidget*> ColumnWidgets;

	// UObject interface.
	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);

	TMultiMap<UObject*, TWeakPtr<SWidget>> CachedRows;

protected:

	TWeakPtr<SHeaderRow> HeaderRow;

};
