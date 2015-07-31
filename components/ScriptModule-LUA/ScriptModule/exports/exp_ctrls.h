#include <control/souictrls.h>


SComboBase * toSComboboxBase(SObject *pObj)
{
    return sobj_cast<SComboBase>(pObj);
}

SComboBox * toSCombobox(SObject *pObj)
{
    return sobj_cast<SComboBox>(pObj);
}

SComboBoxEx * toSComboboxEx(SObject *pObj)
{
    return sobj_cast<SComboBoxEx>(pObj);
}


BOOL ExpLua_Ctrls(lua_State *L)
{
	try{
		lua_tinker::class_add<SComboBase>(L,"SComboBoxBase");
		lua_tinker::class_inh<SComboBase,SWindow>(L);
//         lua_tinker::class_def<SComboBoxBase>(L,"toSWindow",&SComboBoxBase::toSWindow);
		lua_tinker::class_def<SComboBase>(L,"GetCurSel",&SComboBase::GetCurSel);
		lua_tinker::class_def<SComboBase>(L,"GetCount",&SComboBase::GetCount);
		lua_tinker::class_def<SComboBase>(L,"SetCurSel",&SComboBase::SetCurSel);
		lua_tinker::class_def<SComboBase>(L,"GetLBText",&SComboBase::GetLBText);
		lua_tinker::class_def<SComboBase>(L,"FindString",&SComboBase::FindString);
		lua_tinker::class_def<SComboBase>(L,"DropDown",&SComboBase::DropDown);
		lua_tinker::class_def<SComboBase>(L,"CloseUp",&SComboBase::CloseUp);
		lua_tinker::def(L,"toComboboxBase",toSComboboxBase);
		
        lua_tinker::class_add<SComboBox>(L,"SComboBox");
        lua_tinker::class_inh<SComboBox,SComboBase>(L);
        lua_tinker::class_def<SComboBox>(L,"InsertItem",&SComboBox::InsertItem);
        lua_tinker::class_def<SComboBox>(L,"DeleteString",&SComboBox::DeleteString);
        lua_tinker::class_def<SComboBox>(L,"ResetContent",&SComboBox::ResetContent);
        lua_tinker::class_def<SComboBox>(L,"GetLBText",&SComboBox::GetLBText);
        lua_tinker::class_def<SComboBox>(L,"GetListBox",&SComboBox::GetListBox);
        lua_tinker::class_def<SComboBox>(L,"GetItemData",&SComboBox::GetItemData);
        lua_tinker::class_def<SComboBox>(L,"SetItemData",&SComboBox::SetItemData);
        lua_tinker::def(L,"toCombobox",toSCombobox);


        lua_tinker::class_add<SComboBoxEx>(L,"SComboBoxEx");
        lua_tinker::class_inh<SComboBoxEx,SComboBase>(L);
        lua_tinker::class_def<SComboBoxEx>(L,"InsertItem",&SComboBoxEx::InsertItem);
        lua_tinker::class_def<SComboBoxEx>(L,"DeleteString",&SComboBoxEx::DeleteString);
        lua_tinker::class_def<SComboBoxEx>(L,"ResetContent",&SComboBoxEx::ResetContent);
        lua_tinker::class_def<SComboBoxEx>(L,"GetLBText",&SComboBoxEx::GetLBText);
        lua_tinker::class_def<SComboBoxEx>(L,"GetListBox",&SComboBoxEx::GetListBox);
        lua_tinker::class_def<SComboBoxEx>(L,"GetItemData",&SComboBoxEx::GetItemData);
        lua_tinker::class_def<SComboBoxEx>(L,"SetItemData",&SComboBoxEx::SetItemData);
        lua_tinker::def(L,"toComboboxEx",toSComboboxEx);
        
		return TRUE;
	}catch(...)
	{
		return FALSE;
	}
}