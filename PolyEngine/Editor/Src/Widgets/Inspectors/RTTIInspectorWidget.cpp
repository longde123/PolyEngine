#include "PolyEditorPCH.hpp"

#include <Resources/ResourceBase.hpp>
#include <Resources/MeshResource.hpp>

#include "Managers/CommandsImpl.hpp"

RTTIInspectorWidget::RTTIInspectorWidget(QWidget* parent)
	: PolyWidget(parent)
{
	Layout = new QGridLayout(this);
	Layout->setColumnStretch(0, 1);
	Layout->setColumnStretch(1, 3);

	setLayout(Layout);
}

//------------------------------------------------------------------------------
void RTTIInspectorWidget::SetObject(RTTIBase* obj, bool debug)
{
	QLayoutItem* child;

	while ((child = Layout->takeAt(0)) != 0)
		delete child;

	Object = obj;
	ReloadInspector(debug);
}

//------------------------------------------------------------------------------
void RTTIInspectorWidget::UpdateInspector()
{
	for (auto field : Fields)
		field->UpdateControl();
}

//------------------------------------------------------------------------------
void RTTIInspectorWidget::ReloadInspector(bool debug)
{
	Fields.Clear();
	Labels.Clear();
	for (SectionContainer* s : Sections)
		delete s;
	Sections.Clear();

	int row = 0;

	for (auto& child : Object->GetPropertyManager()->GetPropertyList())
	{
		if (!debug && child.Flags.IsSet(RTTI::ePropertyFlag::EDITOR_DEBUG_ONLY))
			continue;

		void* ptr = ((char*)Object) + child.Offset;

		if (child.CoreType == RTTI::eCorePropertyType::CUSTOM)
		{
			Layout->addWidget(AddChild((RTTIBase*)ptr, child, debug), row, 0, 1, 2);
		}
		else
			AddItem(Layout, row, ptr, child);

		++row;
	}
}

//------------------------------------------------------------------------------
void RTTIInspectorWidget::ControlObjectUpdatedSlot(Command* c)
{
	emit ControlObjectUpdated(c);
}

//------------------------------------------------------------------------------
SectionContainer* RTTIInspectorWidget::AddChild(RTTIBase* obj, const RTTI::Property& prop, bool debug)
{
	SectionContainer* section = new SectionContainer(prop.Name.GetCStr(), this);
	Sections.PushBack(section);

	int row = 0;

	for (auto& child : Object->GetPropertyManager()->GetPropertyList())
	{
		if (!debug && child.Flags.IsSet(RTTI::ePropertyFlag::EDITOR_DEBUG_ONLY))
			continue;

		void* ptr = ((char*)Object) + child.Offset;

		if (child.CoreType == RTTI::eCorePropertyType::CUSTOM)
			Layout->addWidget(AddChild((RTTIBase*)ptr, child, debug), row, 0, 1, 2);
		else if (child.CoreType == RTTI::eCorePropertyType::RAW_PTR)
		{
			void* p = *((void**)ptr);

			if (prop.Name == "Mesh")
				AddItem(Layout, row, p, child);
		}
		else
			AddItem(Layout, row, ptr, child);

		++row;
	}

	return section;
}

//------------------------------------------------------------------------------
void RTTIInspectorWidget::AddItem(QGridLayout* parent, int row, void* ptr, const RTTI::Property& prop)
{
	RTTIRegisteredControl* control;

	control = ControlCreatorsManager::CreateControl(this, prop.CoreType);

	control->SetObject(ptr);
	control->SetName(prop.Name);
	control->SetToolTip("SomeCoreType");

	parent->addWidget(control, row, 0, 1, 2);

	connect(control, &RTTIRegisteredControl::ObjectUpdated, this, &RTTIInspectorWidget::ControlObjectUpdatedSlot);

	Fields.PushBack(control);
}