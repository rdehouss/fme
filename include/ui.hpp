/* # -*- coding: utf-8 -*- */
/*
 * Fluxbox Menu Editor
 *
 * ui.hpp
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Author: Raphael Dehousse
 */

#ifndef _UI_HPP_
#define _UI_HPP_

#ifdef HAVE_CONFIG_H
#include <config.hpp>
#endif /* HAVE_CONFIG_H */

#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <list>
#include <gtkmm.h>
#include <libglademm.h>
#ifdef HAVE_LIBBOOST_REGEX
#include <boost/regex.hpp>
#endif /* HAVE_LIBBOOST_REGEX */

#include <Manip.hpp>
#include <Menu.hpp>
#include <TreeModel_DnD.hpp>
#include <Action.hpp>
#include <Preferences.hpp>
#include <ListModelPathsIcons.hpp>
#include <iconview.hpp>

#ifdef ENABLE_NLS
#include <slice_i18n.hpp>
#endif /* ENABLE_NLS */

int compare_path(const Gtk::TreeModel::Path& path1, const Gtk::TreeModel::Path& path2);
int self_submenu(const Gtk::TreeModel::Path& path1, const Gtk::TreeModel::Path& path2);
Gtk::TreePath operator+(const Gtk::TreePath& pathDst, const Gtk::TreePath& pathSrc);
Gtk::TreePath operator-(const Gtk::TreePath& pathDst, const Gtk::TreePath& pathSrc);
Gtk::TreePath plusorless(const Gtk::TreePath& pathDst, const Gtk::TreePath& pathSrc, int op);

class ui : public Gtk::Window {
	private:
		bool in_progress;
		bool gt10rc2;
		bool _edited;
		Action _action;
		Menu *pMenu;
		bool _encoding_changed;
		Preferences preferences;
		Glib::ustring _menuName;
		Glib::ustring icon_selected; /* TODO : Why in global ??? */
		std::list<Action> _listAction;
		std::list<Action>::iterator _iterAction;
		const Glib::RefPtr<Gnome::Glade::Xml>& _xml;
		Gtk::Image preview;

		Gtk::Window *pWindow;
		Gtk::FileChooserDialog *pSelectIcon;
		Gtk::FileChooserDialog *pSelectBin;
		Gtk::Window *pWindowForWait;
		Gtk::ProgressBar *pProgressBar;
		Gtk::Statusbar *pStatusBar;
		Gtk::Label *pLabelMode;
		Gtk::Label *pLabelRowSelected;
		Gtk::ComboBox *pComboBoxWhere;
		Gtk::Table *pTableAdd;
		Gtk::Table *pTableEdit;
		Gtk::Table *pTableDelete;
		Gtk::ComboBox *pComboBoxType;
		Gtk::Entry *pEntryLabel;
		Gtk::Entry *pEntryCmd;
		Gtk::Entry *pEntryIcon;
		Gtk::Button *pBtnAdd;
		Gtk::Button *pBtnEdit;
		Gtk::Button *pBtnDelete;
		Gtk::Button *pBtnIcon;
		Gtk::Button *pBtnSave;
		Gtk::Button *pBtnUndo;
		Gtk::Button *pBtnRedo;
		Gtk::Button *pBtnBrowseIcons; /* TODO : Necessary ? */
		Gtk::Dialog *pIconViewDialog;
		Gtk::IconView *pIconView;
		Gtk::Entry *pEntryMenuName;
		Gtk::TreeView *pTreeView;
		Gtk::TreeView *pTreeViewPathsIcons;
		Gtk::FileChooserButton *pSelectPathIcons;

		Glib::RefPtr<Gtk::UIManager> pUIManager;
		Glib::RefPtr<Gtk::ActionGroup> pActionGroup;
		Gtk::Menu *pContextMenu;

		Glib::RefPtr<TreeModel_DnD> pTreeModel;
		Glib::RefPtr<Gtk::ListStore> pListModel;
		Glib::RefPtr<Gtk::ListStore> pListModelIconView;
		Glib::RefPtr<ListModelPathsIcons> pListModelPathsIcons;

		class ModelColumnsCompletion : public Gtk::TreeModel::ColumnRecord {
			public:
				Gtk::TreeModelColumn<unsigned int> id;
				Gtk::TreeModelColumn<Glib::ustring> label;

				ModelColumnsCompletion(void) { add(id); add(label); }
		};

		ModelColumnsCompletion CompletionColumns;

		ModelColumnsIcon IconsColumns;

	public:
		ui(BaseObjectType* base_object, const Glib::RefPtr<Gnome::Glade::Xml>& xml);
		~ui(void);

		const bool isFluxboxGt10rc2(void) const { return gt10rc2; }
		const Glib::ustring& getMenuName(void) const { return _menuName; }
		void setMenuName(const Glib::ustring& menuName) { _menuName = menuName; }

		void _init_ui_(void);
		void _init_widgets_(void);
		void _init_contextMenu_(void);
		void _connect_sig_(void);
		void _init_prefs_dialog_(void);
		void load_menu(const Glib::ustring filename="");
		void load_submenu(Gtk::TreeModel::Row *row);

		void show_error(const Glib::ustring& str, const Glib::ustring& str_secondary);
		bool confirm_exit(void);
		void save_submenu(std::ostream& s, int &iEnfant, const Gtk::TreeNodeChildren& child, std::string parent_type);
		void menu_edited(bool edited, bool reg=false);
		void copy_row(Gtk::TreeModel::Row& row, const MenuLine& line);
		void copy_row(MenuLine& line, const Gtk::TreeModel::Row& row);
		int copy_row(Gtk::TreeModel::Row& rowNew, const Gtk::TreeModel::Row& rowOld);
		int add_row(const Gtk::TreeModel::Row& row, int& NumberChildren);
		int remove_row(const Gtk::TreeStore::iterator& iterToErase, int& size);
		int getNumberChildren(const Gtk::TreeStore::iterator& iter);
		void en_dis_able_some_buttons(void);
		void hl(void);
		void hl(const Gtk::TreeModel::Path& path);
		void generate_menu(void);
		void getPathFiles(void);
		bool pulse(void);

		bool on_key_realeased(GdkEventKey* key);
		bool on_search_equal(const Glib::RefPtr<Gtk::TreeModel>& model, int column, const Glib::ustring& key, const Gtk::TreeModel::iterator& iter);
		bool on_delete_event(GdkEventAny *event);
		void on_row_expanded(const Gtk::TreeModel::iterator& iter, const Gtk::TreeModel::Path& path);
		void on_row_collapsed(const Gtk::TreeModel::iterator& iter, const Gtk::TreeModel::Path& path);
		void on_row_activated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);
		bool on_EntryMenuName_changed(GdkEventFocus *focus);
		void on_entryIcon_changed(void);
		void show_tableAdd(void);
		void edit(bool show_pane=true);
		void on_update_preview(void);
		void on_button_pressed(GdkEventButton* event);
		void on_collapse_all(void);
		void on_expand_all(void);
		void on_completion_action_activated(int);

		/* Handler for DnD */
		void on_drag_begin(const Glib::RefPtr<Gdk::DragContext>& context);
		void on_row_inserted(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter);
		void on_drag_end(const Glib::RefPtr<Gdk::DragContext>& context);

		void search_file_in_paths(void);

		void on_btnNew_clicked(void);
		void on_btnAdd_clicked(void);
		void on_btnEdit_clicked(void);
		void on_btnDelete_clicked(void);
		void on_btnIcon_clicked(void);
		void on_btnSearchIcon_clicked(void);
		void on_btnSelectBin_clicked(void);
		void on_btnUndo_clicked(void);
		void on_btnRedo_clicked(void);
		void on_btnPref_clicked(void);
		void on_btnSave_clicked(void);
		void on_btnAbout_clicked(void);
		void on_btnQuit_clicked(void);

		void on_btnConfirmAdd_clicked(void);
		void on_btnHideAdd_clicked(void);
		void on_btnConfirmEdit_clicked(void);
		void on_btnHideEdit_clicked(void);
		void on_btnConfirmDelete_clicked(void);
		void on_btnHideDelete_clicked(void);

		void on_btnAddPathIcons_clicked(void);
		void on_btnDelPathIcons_clicked(void);

		void on_selection_changed(void);
};

#endif /* _UI_HPP_ */
