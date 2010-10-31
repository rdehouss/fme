/* # -*- coding: utf-8 -*- */
/*
 * Fluxbox Menu Editor
 *
 * ui.cc
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

#include <ui.hpp>
#include <errno.h>

int compare_path(const Gtk::TreeModel::Path& path1, const Gtk::TreeModel::Path& path2) {
	int iCompare = path1.to_string().compare(path2.to_string());

	if ( iCompare==1 ) {
		if ( path1.size()==path2.size() ) {
			if ( !self_submenu(path1, path2) ) return 0;
		}
	}

	return iCompare;
}

int self_submenu(const Gtk::TreeModel::Path& path1, const Gtk::TreeModel::Path& path2) {
	int iReturn = !( path1.size()==path2.size() );
	Gtk::TreeModel::Path::const_iterator iter1 = path1.end();
	Gtk::TreeModel::Path::const_iterator iter2 = path2.end();

	iter1--;
	iter2--;
	while ( iter1!=path1.begin() && iter2!=path2.begin() ) {
		iter1--;
		iter2--;
		// not self submenu
		if ( (*iter1-*iter2) ) iReturn = (*iter1-*iter2);
	}
	// if iReturn==0 : self submenu
	return iReturn;
}

Gtk::TreePath operator+(const Gtk::TreePath& pathDst, const Gtk::TreePath& pathSrc) {
	return plusorless(pathDst, pathSrc, 1);
}

Gtk::TreePath operator-(const Gtk::TreePath& pathDst, const Gtk::TreePath& pathSrc) {
	return plusorless(pathDst, pathSrc, -1);
}

Gtk::TreePath plusorless(const Gtk::TreePath& pathDst, const Gtk::TreePath& pathSrc, int op) {
	int index;
	Gtk::TreePath path;
	Gtk::TreePath::const_iterator iter;

	if ( compare_path(pathDst, pathSrc)<0 ) {
		iter = pathSrc.begin();
		for ( int i=1; iter!=pathSrc.end(); i++, iter++ ) {
			index = *iter;
			if ( i==pathDst.size() ) index += op;
			if ( index>=0 ) path.append_index(index);
		}
	}
	else {
		iter = pathDst.begin();
		for ( int i=1; iter!=pathDst.end(); i++, iter++ ) {
			index = *iter;
			if ( i==pathSrc.size() ) index += op;
			if ( index>=0 ) path.append_index(index);
		}
	}

	return path;
}

ui::ui(BaseObjectType* base_object, const Glib::RefPtr<Gnome::Glade::Xml>& xml) : Gtk::Window(base_object), _xml(xml) {
	std::clog << " * ui constructor" << std::endl;

	/* Check version */
	char szgt10rc2[2] = {0};
	FILE *fdsh = popen("fme_check-ver_fluxbox", "r");
	if ( NULL==fgets(szgt10rc2, 2, fdsh) ) {
		std::cerr << "Cannot read version of Fluxbox with fme_check-ver_fluxbox" << std::endl;
	}
	pclose(fdsh);
	gt10rc2 = atoi(szgt10rc2);
	std::clog << "Greater than 1.0rc2 ? " << gt10rc2 << std::endl;

	_init_ui_();
	_init_widgets_();
	_init_prefs_dialog_();

	load_menu();
	try {
		std::clog << tab << "create a work copy of the menu" << std::endl;
		copy(( std::string(pMenu->getFilename_c()) + ".fme.workbak" ).c_str(), pMenu->getFilename_c());
	} catch ( FileNotExist ) {
		std::clog << tab << "File \"" << pMenu->getFilename_c() << "\" don't exist" << std::endl;
	} catch ( CannotOpenFile ) {
		show_error(_("Menu can't be copied"), _("Not enough disk space ?"));
		exit(EXIT_SUCCESS);
	}

	if ( (pMenu->getEncoding()).empty() ) {
		pMenu->setEncoding("ISO8859-1");
	}

	_connect_sig_();

	pTreeView->set_model(pTreeModel);
	pTreeViewPathsIcons->set_model(pListModelPathsIcons);
	hl(); // Highlight the first row
}

ui::~ui(void) {
	std::clog << " * ui destructor" << std::endl;

	if ( !_listAction.size() && !_encoding_changed ) {
		std::ifstream file(( std::string(pMenu->getFilename_c()) + ".fme.workbak" ).c_str());
		if ( file.is_open() ) {
			file.close();
			std::clog << tab << "rename work copy in menu file" << std::endl;
			rename(( std::string(pMenu->getFilename_c()) + ".fme.workbak" ).c_str(), pMenu->getFilename_c());
		}
	}
	else {
		std::ifstream file(( std::string(pMenu->getFilename_c()) + ".fme.workbak" ).c_str());
		if ( file.is_open() ) {
			file.close();
			std::clog << tab << "rename work copy in bak file" << std::endl;
			rename(( std::string(pMenu->getFilename_c()) + ".fme.workbak" ).c_str(), ( std::string(pMenu->getFilename_c()) + ".fme.bak" ).c_str());
		}
	}

	if ( pMenu ) delete pMenu;

	std::clog << "rm -f /tmp/fmemenu.*" << std::endl;
	FILE *fdsh = popen("rm -f /tmp/fmemenu.*", "r");
	pclose(fdsh);
}

void ui::_init_ui_(void) {
	std::clog << " * _init_ui_" << std::endl;

	_edited = false;
	_encoding_changed = false;

	pTreeModel = TreeModel_DnD::create();

	pListModelPathsIcons = ListModelPathsIcons::create();

	_xml->get_widget("MainWindow", pWindow);
	_xml->get_widget("iconChooserDialog", pSelectIcon);
	_xml->get_widget("iconViewDialog", pIconViewDialog);
	_xml->get_widget("binChooserDialog", pSelectBin);
	_xml->get_widget("windowForWait", pWindowForWait);
	_xml->get_widget("progressbar", pProgressBar);

	_xml->get_widget("entryMenuName", pEntryMenuName);
	_xml->get_widget("TreeView", pTreeView);

	_xml->get_widget("iconviewIconFound", pIconView);

	_xml->get_widget("TreeViewPathsIcons", pTreeViewPathsIcons);
	_xml->get_widget("filechooserbuttonSelectPathIcon", pSelectPathIcons);

	_xml->get_widget("labelMode", pLabelMode);
	_xml->get_widget("labelRowSelected", pLabelRowSelected);
	_xml->get_widget("comboboxWhere", pComboBoxWhere);
	_xml->get_widget("tableAdd", pTableAdd);
	_xml->get_widget("tableEdit", pTableEdit);
	_xml->get_widget("tableDelete", pTableDelete);

	_xml->get_widget("comboboxType", pComboBoxType);
	_xml->get_widget("entryLabel", pEntryLabel);
	_xml->get_widget("entryCmd", pEntryCmd);
	_xml->get_widget("entryIcon", pEntryIcon);

	_xml->get_widget("btnEdit", pBtnEdit);
	_xml->get_widget("btnDelete", pBtnDelete);
	_xml->get_widget("btnIcon", pBtnIcon);
	_xml->get_widget("btnUndo", pBtnUndo);
	_xml->get_widget("btnRedo", pBtnRedo);
	_xml->get_widget("btnSave", pBtnSave);

	_xml->get_widget("StatusBar", pStatusBar);

	try {
		pMenu = new Menu();
	} catch ( EnvPathNotDefined ) {
		show_error(_("HOME dir not found"), _("Please fix it"));
		exit(EXIT_SUCCESS);
	} catch ( FluxboxNotFound ) {
		show_error(_("~/.fluxbox/ not found"), _("Was Fluxbox already started once ?"));
		exit(EXIT_SUCCESS);
	} catch ( ... ) {
		show_error(_("Menu can't be load"), _("The application will be closed"));
		exit(EXIT_SUCCESS);
	}
}

void ui::_init_widgets_(void) {
	std::clog << " * _init_widgets_" << std::endl;
	pWindow->set_title(PACKAGE_STRING" " + pMenu->getFilename());

	pWindowForWait->set_transient_for(*pWindow);

	Gtk::TreeView::Column* pColumn;
	
	pTreeView->append_column(_("Icon"), pTreeModel->Columns.pixbuf);
	pColumn = pTreeView->get_column(0);
	pColumn->set_min_width(70);
	pColumn->set_resizable();

	pTreeView->append_column(_("Type"), pTreeModel->Columns.type);
	pColumn = pTreeView->get_column(1);
	pColumn->set_min_width(90);
	pColumn->set_resizable();

	pTreeView->append_column(_("Label"), pTreeModel->Columns.label);
	pColumn = pTreeView->get_column(2);
	pColumn->set_min_width(100);
	pColumn->set_resizable();

	pTreeView->append_column(_("Command"), pTreeModel->Columns.cmd);
	pColumn = pTreeView->get_column(3);
	pColumn->set_min_width(100);
	pColumn->set_resizable();
	
	pTreeView->columns_autosize();
	pTreeView->set_search_equal_func(sigc::mem_fun(*this, &ui::on_search_equal));

	pTreeViewPathsIcons->append_column(_("Path"), pListModelPathsIcons->Columns.path);

	/* Context Menu */
	_init_contextMenu_();

	pIconViewDialog->set_transient_for(*pWindow);

	pSelectIcon->set_transient_for(*pWindow);
	Gtk::FileFilter filter;
	filter.set_name(_("Images"));
	filter.add_mime_type("image/png");
	filter.add_pattern("*.xpm");
	filter.add_pattern("*.png");
	pSelectIcon->add_filter(filter);

	pSelectIcon->set_preview_widget(preview);
	pSelectIcon->set_preview_widget_active(true);
}

void ui::_init_contextMenu_(void) {
	std::clog << " * _init_contextMenu_ " << std::endl;

	pActionGroup = Gtk::ActionGroup::create();

	pActionGroup->add(Gtk::Action::create("ContextExpand", _("Expand all")), sigc::mem_fun(*this, &ui::on_expand_all));
	pActionGroup->add(Gtk::Action::create("ContextCollapse", _("Collapse all")), sigc::mem_fun(*this, &ui::on_collapse_all));

	pUIManager = Gtk::UIManager::create();
	pUIManager->insert_action_group(pActionGroup);

	add_accel_group(pUIManager->get_accel_group());

	Glib::ustring ui_info =
		"<ui>"
		"	<popup name='PopupMenu'>"
		"		<menuitem action='ContextExpand' />"
		"		<menuitem action='ContextCollapse' />"
		"	</popup>"
		"</ui>";

	try {
		pUIManager->add_ui_from_string(ui_info);
	} catch ( std::exception&  e) {
		std::clog << " !! building menu failed : " << e.what() << std::endl;
	}

	pContextMenu = dynamic_cast<Gtk::Menu*>(pUIManager->get_widget("/PopupMenu") );
	if ( !pContextMenu ) {
		g_warning("menu not found");
	}
}

void ui::_init_prefs_dialog_(void) {
	std::clog << " * _init_prefs_dialog_" << std::endl;

	Gtk::TreeModel::Row row;
	Gtk::ListStore::iterator iter;
	std::vector<Glib::ustring> paths_icons = preferences.getPathsIcons();
	std::vector<Glib::ustring>::iterator iterPath;
	
	for ( iterPath = paths_icons.begin(); iterPath!=paths_icons.end(); iterPath++ ) {
		iter = pListModelPathsIcons->append();
		row = *iter;
		row[pListModelPathsIcons->Columns.path] = *iterPath;
	}
}

void ui::_connect_sig_(void) {
	std::clog << " * _connect_sig_" << std::endl;

	pTreeView->signal_key_release_event().connect(sigc::mem_fun(*this, &ui::on_key_realeased));

	pEntryMenuName->signal_focus_out_event().connect(sigc::mem_fun(*this, &ui::on_EntryMenuName_changed));
	pTreeView->signal_row_expanded().connect(sigc::mem_fun(*this, &ui::on_row_expanded));
	pTreeView->signal_row_collapsed().connect(sigc::mem_fun(*this, &ui::on_row_collapsed));
	pTreeView->signal_row_activated().connect(sigc::mem_fun(*this, &ui::on_row_activated));
	pTreeView->signal_cursor_changed().connect(sigc::mem_fun(*this, &ui::en_dis_able_some_buttons));

	/* Signals for DnD */
	pTreeView->signal_drag_begin().connect(sigc::mem_fun(*this, &ui::on_drag_begin));
	pTreeModel->signal_row_inserted().connect(sigc::mem_fun(*this, &ui::on_row_inserted));
	pTreeView->signal_drag_end().connect(sigc::mem_fun(*this, &ui::on_drag_end));

	/* Context Menu */
	pTreeView->signal_button_press_event().connect_notify(sigc::mem_fun(*this, &ui::on_button_pressed));

	/* Signal for FileChooser */
	pSelectIcon->signal_update_preview().connect(sigc::mem_fun(*this, &ui::on_update_preview));

	/* Signal for IconView */
	pIconView->signal_selection_changed().connect(sigc::mem_fun(*this, &ui::on_selection_changed));

	_xml->connect_clicked("btnNew", sigc::mem_fun(*this, &ui::on_btnNew_clicked));
	_xml->connect_clicked("btnAdd", sigc::mem_fun(*this, &ui::on_btnAdd_clicked));
	_xml->connect_clicked("btnEdit", sigc::mem_fun(*this, &ui::on_btnEdit_clicked));
	_xml->connect_clicked("btnDelete", sigc::mem_fun(*this, &ui::on_btnDelete_clicked));
	_xml->connect_clicked("btnIcon", sigc::mem_fun(*this, &ui::on_btnIcon_clicked));
	_xml->connect_clicked("btnSearchIcon", sigc::mem_fun(*this, &ui::on_btnSearchIcon_clicked));
	_xml->connect_clicked("btnSelectBin", sigc::mem_fun(*this, &ui::on_btnSelectBin_clicked));
	_xml->connect_clicked("btnUndo", sigc::mem_fun(*this, &ui::on_btnUndo_clicked));
	_xml->connect_clicked("btnRedo", sigc::mem_fun(*this, &ui::on_btnRedo_clicked));
	_xml->connect_clicked("btnPref", sigc::mem_fun(*this, &ui::on_btnPref_clicked));
	_xml->connect_clicked("btnSave", sigc::mem_fun(*this, &ui::on_btnSave_clicked));
	_xml->connect_clicked("btnAbout", sigc::mem_fun(*this, &ui::on_btnAbout_clicked));
	_xml->connect_clicked("btnQuit", sigc::mem_fun(*this, &ui::on_btnQuit_clicked));

	_xml->connect_clicked("btnConfirmAdd", sigc::mem_fun(*this, &ui::on_btnConfirmAdd_clicked));
	_xml->connect_clicked("btnHideAdd", sigc::mem_fun(*this, &ui::on_btnHideAdd_clicked));
	_xml->connect_clicked("btnConfirmEdit", sigc::mem_fun(*this, &ui::on_btnConfirmEdit_clicked));
	_xml->connect_clicked("btnHideEdit", sigc::mem_fun(*this, &ui::on_btnHideEdit_clicked));
	_xml->connect_clicked("btnChooseIcon", sigc::mem_fun(*this, &ui::on_btnIcon_clicked));
	_xml->connect_clicked("btnConfirmDelete", sigc::mem_fun(*this, &ui::on_btnConfirmDelete_clicked));
	_xml->connect_clicked("btnHideDelete", sigc::mem_fun(*this, &ui::on_btnHideDelete_clicked));

	_xml->connect_clicked("btnAddPathIcons", sigc::mem_fun(*this, &ui::on_btnAddPathIcons_clicked));
	_xml->connect_clicked("btnDelPathIcons", sigc::mem_fun(*this, &ui::on_btnDelPathIcons_clicked));

	_xml->connect_clicked("btnBrowseIcons", sigc::mem_fun(*this, &ui::on_btnIcon_clicked));
}

void ui::load_menu(const Glib::ustring filename) {
	std::clog << " * load_menu" << " " << filename << std::endl;

	if ( filename=="none" ) return;

	try {
		pMenu->open(filename);
	} catch ( CannotOpenFile& e ) {
		std::clog << "Cannot open menu file" << std::endl;
		show_error(_("Menu can't be load"), _("The application will be closed"));
		exit(EXIT_SUCCESS);
	}

	std::clog << " * load_submenu" << std::endl;
	load_submenu(NULL);

	try {
		pMenu->close();
	} catch ( CannotCloseFile& e ) {
		std::clog << "Cannot close menu file" << std::endl;
	}
}

static int submenu = 0; // Only for debugging

void ui::load_submenu(Gtk::TreeModel::Row *row) {
	submenu++; // Only for debugging
	bool bEnd=false;
	MenuLine line;
	Gtk::TreeModel::Row childrow;

	while ( !pMenu->eof() && !bEnd ) {
		line = pMenu->getLine();

		if ( line.type=="begin" ) {
			/* _FLUXBOX_GT_1_0_RC2_ */
			try {
				/* To be honest, I don't know why it works... */
				if ( isFluxboxGt10rc2() && Glib::get_charset() ) {
					setMenuName(Glib::convert(line.label, "ISO-8859-15", "UTF-8"));
				}
				else {
					if ( isFluxboxGt10rc2() )
						setMenuName(line.label);
					else
						setMenuName(Glib::convert(line.label, "ISO-8859-15", "UTF-8"));
				}
			} catch ( Glib::ConvertError ) {
				std::clog << " !! ConvertError" << std::endl;
				setMenuName(line.label);
			}
			/* _FLUXBOX_GT_1_0_RC2_ */
			pEntryMenuName->set_text(getMenuName());
			std::clog << tab(submenu++) << line << std::endl;
		}
		else {
			if ( line.type=="end" ) {
				bEnd = true;
				std::clog << tab(--submenu) << line << std::endl;
			}
			else {
				if ( line.type=="encoding" || line.type=="endencoding" ) {
					if ( line.type!="endencoding" ) {
						pMenu->setEncoding(line.cmd);
					}
				}
				else {
					if ( line.type!="" ) {
						std::clog << tab(submenu) << line << std::endl;
						Gtk::TreeStore::iterator iter;
						if ( row==NULL ) {
							iter = pTreeModel->append();
						}
						else {
							iter = pTreeModel->append(row->children());
						}
						childrow = *iter;

						if ( !line.icon.empty() ) {
							std::ifstream file(line.icon.c_str());
							if ( file.is_open() ) {
								file.close();
								childrow[pTreeModel->Columns.pixbuf] = Gdk::Pixbuf::create_from_file(line.icon, 15, 15);
							}
						}

#ifdef GLIBMM_REGEX
						Glib::RefPtr<Glib::Regex> regex = Glib::Regex::create(
#else
						boost::regex expr(
#endif /* GLIBMM_REGEX */
						"(begin|end|exec|exit|include|nop|separator|style|stylesdir|stylesmenu|submenu|reconfig|restart|config|workspaces|commanddialog)"
#ifdef GLIBMM_REGEX
						, Glib::REGEX_CASELESS);
						if ( !regex->match(line.type) ) {
#else
						, boost::regex_constants::icase);
						if ( !boost::regex_search(line.type.c_str(), expr) ) {
#endif /* GLIBMM_REGEX */
							std::clog << tab << "Syntax error" << std::endl;
							/* TODO : show a popup with this message ? */
						}

						childrow[pTreeModel->Columns.type] = line.type;
						childrow[pTreeModel->Columns.label] = line.label;
						childrow[pTreeModel->Columns.cmd] = line.cmd;
						childrow[pTreeModel->Columns.icon] = line.icon;

						if ( line.type=="submenu" ) {
							load_submenu(&childrow);
						}
					}
				}
			}
		}
	}
}


void ui::show_error(const Glib::ustring& str, const Glib::ustring &str_secondary) {
	Gtk::MessageDialog dialog(*pWindow, str, false, Gtk::MESSAGE_ERROR);

	dialog.set_secondary_text(str_secondary);

	dialog.run();
}

bool ui::confirm_exit(void) {
	std::clog << " * confirm_exit : " << _edited << std::endl;
	if ( !_edited ) return true;

	Gtk::Dialog *pDialog = NULL;
	_xml->get_widget("dialogQuit", pDialog);
	pDialog->set_transient_for(*pWindow);

	if ( pDialog->run()==Gtk::RESPONSE_OK ) return true;

	pDialog->hide();

	return false;
}

void ui::save_submenu(std::ostream& s, int &iEnfant, const Gtk::TreeNodeChildren& child, std::string parent_type) {
	bool end = false;
	MenuLine line;
	Gtk::TreeModel::Row row;
	Gtk::TreeModel::Children::iterator iter;

	iEnfant++;

	line.encoding = pMenu->getEncoding();

	for ( iter=child.begin(); iter!=child.end(); iter++ ) {
		end = true;
		row = *iter;
		copy_row(line, row);

		s << tab(iEnfant) << line << std::endl;
		std::clog << tab(iEnfant) << line << std::endl;

		save_submenu(s, iEnfant, row.children(), line.type);
	}

	iEnfant--;

	if ( (end && iEnfant) || parent_type.find("menu")!=std::string::npos ) {
		s << tab(iEnfant) << "[end]" << std::endl;
		std::clog << tab(iEnfant) << "[end]" << std::endl;
	}
}

void ui::menu_edited(bool edited, bool reg) {
	std::clog << " * menu_edited" << std::endl;
	const Glib::ustring title = pWindow->get_title();

	if ( edited ) {
		_edited = true;
		pBtnSave->set_sensitive(true);
		if ( title.compare(0, 1, "*") )
			pWindow->set_title("*" + title);
		if ( reg ) {
			std::clog << tab << "push_front(_action)" << std::endl;
			std::list<Action>::iterator iter = _listAction.begin();
			while ( iter!=_iterAction && iter!=_listAction.end() ) {
				_listAction.erase(iter);
				iter = _listAction.begin();
			}
			_listAction.push_front(_action);
			_action.clean();
			_iterAction = _listAction.begin();
			pBtnUndo->set_sensitive(true);
			pBtnRedo->set_sensitive(false);
		}
		pStatusBar->pop();
		pStatusBar->push(_("* Menu edited *"));
	}
	else {
		if ( reg ) {
			pBtnSave->set_sensitive(true);
			_encoding_changed = true;
		}
		_edited = false;
		if ( !title.compare(0, 1, "*") )
			pWindow->set_title(title.substr(1));
		pStatusBar->pop();
		pStatusBar->push(_("* Menu saved *"));
	}
}

void ui::copy_row(Gtk::TreeModel::Row& row, const MenuLine& line) {
	row[pTreeModel->Columns.type] = line.type;
	row[pTreeModel->Columns.label] = line.label;
	row[pTreeModel->Columns.cmd] = line.cmd;
	row[pTreeModel->Columns.icon] = line.icon;
	if ( line.icon.size() ) {
		Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create_from_file(line.icon, 15, 15);
		row[pTreeModel->Columns.pixbuf] = pixbuf;
	}
	else {
		Glib::RefPtr<Gdk::Pixbuf> pixbuf;
		pixbuf.clear();
		row[pTreeModel->Columns.pixbuf] = pixbuf;
	}
}

void ui::copy_row(MenuLine& line, const Gtk::TreeModel::Row& row) {
	line.type = row[pTreeModel->Columns.type];
	line.label = row[pTreeModel->Columns.label];
	line.cmd = row[pTreeModel->Columns.cmd];
	line.icon = row[pTreeModel->Columns.icon];
}

int ui::copy_row(Gtk::TreeModel::Row& rowNew, const Gtk::TreeModel::Row& rowOld) {
	Glib::RefPtr<Gdk::Pixbuf> pixbuf = rowOld[pTreeModel->Columns.pixbuf];
	if ( pixbuf ) {
		rowNew[pTreeModel->Columns.pixbuf] = pixbuf->copy();
	}
	Glib::ustring str = rowOld[pTreeModel->Columns.type];
	rowNew[pTreeModel->Columns.type] = str;
	str = rowOld[pTreeModel->Columns.label];
	rowNew[pTreeModel->Columns.label] = str;
	str = rowOld[pTreeModel->Columns.cmd];
	rowNew[pTreeModel->Columns.cmd] = str;
	str = rowOld[pTreeModel->Columns.icon];
	rowNew[pTreeModel->Columns.icon] = str;

	Gtk::TreeNodeChildren child = rowOld.children();
	Gtk::TreeModel::Children::iterator iterChild;
	Gtk::TreeModel::Row rowNewSave = rowNew;

	for ( Gtk::TreeModel::Children::iterator iter=child.begin(); iter!=child.end(); iter++ ) {
		iterChild = pTreeModel->append(rowNewSave.children());
		rowNew = *iterChild;
		copy_row(rowNew, *iter);
	}

	return child.size();
}

int ui::add_row(const Gtk::TreeModel::Row& row, int& NumberChildren) {
	_iterAction++;

	Gtk::TreeNodeChildren child = pTreeModel->children();
	Gtk::TreeModel::Path pathFather = _action.getPathSrc();
	if ( _iterAction!=_listAction.end() ) {
		_action = *_iterAction;
		Gtk::TreeModel::Path pathChild = _action.getPathSrc();

		while ( NumberChildren>0 && pathChild.is_descendant(pathFather) ) {
			child = row.children();
			Gtk::TreeModel::Row rowChild = *(pTreeModel->append(child));
			copy_row(rowChild, _action.getMenuLineOld());
			NumberChildren--;
			add_row(rowChild, NumberChildren);
			pathChild = _action.getPathSrc();
		}
	}
}

int ui::remove_row(const Gtk::TreeStore::iterator& iterToErase, int& numberChildren) {
	MenuLine line;
	Gtk::TreeModel::Row row = *iterToErase;
	Gtk::TreeNodeChildren child = row.children();
	Gtk::TreeModel::Children::iterator iter = child.end();

	if ( iter!=child.begin() ) {
		for ( --iter; 17; iter--) {
			_action.clean();
			_action.setType(ROW_DELETED);

			remove_row(iter, numberChildren);

			row = *iter;
			copy_row(line, row);
			_action.setMenuLineOld(line);

			_action.setPathSrc(pTreeModel->get_path(iter));
			_action.setNumberChildren(numberChildren);
			menu_edited(true, true);
			numberChildren--;
			if ( iter==child.begin() ) break;
		}
	}

	return numberChildren;
}

int ui::getNumberChildren(const Gtk::TreeStore::iterator& iterToCount) {
	Gtk::TreeModel::Row row = *iterToCount;
	Gtk::TreeNodeChildren child = row.children();
	int numberChild = child.size();
	Gtk::TreeModel::Children::iterator iter = child.end();

	if ( iter!=child.begin() ) {
		for ( --iter; 17; iter--) {
			numberChild += getNumberChildren(iter);
			if ( iter==child.begin() ) break;
		}
	}

	return numberChild;
}

void ui::en_dis_able_some_buttons(void) {
	Glib::RefPtr<Gtk::TreeSelection> selection = pTreeView->get_selection();
	Gtk::TreeStore::iterator iterTarget = selection->get_selected();

	pBtnEdit->set_sensitive( (!iterTarget)?false:true );
	pBtnDelete->set_sensitive( (!iterTarget)?false:true );
	pBtnIcon->set_sensitive( (!iterTarget)?false:true );

	if ( pTableAdd->is_visible() ) show_tableAdd();
	pTableEdit->hide();
	pTableDelete->hide();
}

void ui::hl(void) {
	pWindow->set_focus(*pTreeView);
}

void ui::hl(const Gtk::TreeModel::Path& path) {
	pTreeView->set_cursor(path);
	hl();
}

void ui::generate_menu(void) {
	std::clog << " * generate_menu" << std::endl;
	in_progress = true;
	Glib::spawn_command_line_sync("fluxbox-generate_menu");
	in_progress = false;
	std::clog << tab << "end of generate_menu" << std::endl;

	load_menu();

	/* copy fgm menu to tmp */
	copy("/tmp/fmemenu.fgm", pMenu->getFilename_c());

	/* restore old menu */
	copy(pMenu->getFilename_c(), "/tmp/fmemenu.tmp");
	/* delete temp old menu */
	unlink("/tmp/fmemenu.tmp");
}

bool ui::on_search_equal(const Glib::RefPtr<Gtk::TreeModel>& model, int column, const Glib::ustring& key, const Gtk::TreeModel::iterator& iter) {
	Gtk::TreeModel::Row row = *iter;

	if ( row[pTreeModel->Columns.type]=="submenu" ) {
		pTreeView->expand_to_path(pTreeModel->get_path(iter));
	}

	Glib::ustring str = row[pTreeModel->Columns.label];
	if ( !str.compare(0, key.size(), key) ) {
		pTreeView->collapse_all();
		pTreeView->expand_to_path(pTreeModel->get_path(iter));
		return false;
	}

	return true;
}

bool ui::on_delete_event(GdkEventAny *event) {
	return !confirm_exit();
}

bool ui::on_key_realeased(GdkEventKey* key) {
	if ( key->keyval==GDK_Delete ) {
		on_btnDelete_clicked();
	}

	return false;
}

void ui::on_row_expanded(const Gtk::TreeModel::iterator& iter, const Gtk::TreeModel::Path& path) {
	pTreeView->columns_autosize();
}

void ui::on_row_collapsed(const Gtk::TreeModel::iterator& iter, const Gtk::TreeModel::Path& path) {
	pTreeView->columns_autosize();
}

void ui::on_row_activated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column) {
	on_btnEdit_clicked();
}

void ui::on_button_pressed(GdkEventButton* event) {
	std::clog << " * on_button_pressed" << std::endl;

	if ( event->type==GDK_BUTTON_PRESS && event->button==3 ) {
		std::clog << tab << "right click" << std::endl;
		pContextMenu->popup(event->button, event->time);
	}
}

void ui::on_collapse_all(void) {
	pTreeView->collapse_all();
}

void ui::on_expand_all(void) {
	pTreeView->expand_all();
}

void ui::on_btnSelectBin_clicked(void) {
	int response = pSelectBin->run();

	if ( response!=Gtk::RESPONSE_CANCEL ) {
		Glib::ustring bin_selected = pSelectBin->get_filename();
		if ( !bin_selected.empty() || response!=Gtk::RESPONSE_OK  ) {
			if ( response==Gtk::RESPONSE_OK ) {
				pEntryCmd->set_text(bin_selected);
			}
			else {
				pEntryCmd->set_text("");
			}
		}
	}

	pSelectBin->hide();
}

void ui::on_completion_action_activated(int index) {
	if ( index==0 ) {
		on_btnSelectBin_clicked();
	}
}

bool ui::on_EntryMenuName_changed(GdkEventFocus *focus) {
	std::clog << " * on_EntryMenuName_changed" << std::endl;
	Glib::ustring strOld, strNew;
	_action.clean();
	_action.setType(TITLE_CHANGED);

	if ( !(strNew=pEntryMenuName->get_text()).empty() && !(strOld=getMenuName()).empty() ) {
		if ( strNew.compare(strOld)) {
			std::clog << tab << "title changed" << std::endl;
			_action.setTitleOld(strOld);
			_action.setTitleNew(strNew);
			menu_edited(true, true);
			setMenuName(strNew);
		}
	}
	else {
		if ( !strNew.empty() ) {
			std::clog << tab << "title changed" << std::endl;
			_action.setTitleOld(strOld);
			_action.setTitleNew(strNew);
			menu_edited(true, true);
			setMenuName(strNew);
		}
	}

	return false;
}

void ui::show_tableAdd(void) {
	std::clog << " * show_tableAdd" << std::endl;
	/* the row selected */
	Glib::RefPtr<Gtk::TreeSelection> selection = pTreeView->get_selection();
	Gtk::TreeStore::iterator iterTarget = selection->get_selected();
	Gtk::TreeModel::Row row;

	if ( !iterTarget ) {
		on_btnConfirmAdd_clicked();
	}
	else {
		row = *iterTarget;

		pComboBoxWhere->hide();
		if ( row[pTreeModel->Columns.type]=="submenu" ) {
			pLabelRowSelected->set_text(_("the submenu selected"));
			_xml->get_widget("comboboxWhereSubmenu", pComboBoxWhere);
		}
		else {
			pLabelRowSelected->set_text(_("the row selected"));
			_xml->get_widget("comboboxWhere", pComboBoxWhere);
		}

		pComboBoxWhere->set_active(0);
		pComboBoxWhere->show();

		pTableAdd->show();
	}
}

void ui::getPathFiles(void) {
	std::clog << " * getPathFiles" << std::endl;

	// Completion
	Glib::RefPtr<Gtk::EntryCompletion> refCompletion = Gtk::EntryCompletion::create();
	pEntryCmd->set_completion(refCompletion);

	Glib::RefPtr<Gtk::ListStore> refCompletionModel = Gtk::ListStore::create(CompletionColumns);
	refCompletion->set_model(refCompletionModel);

	refCompletion->set_minimum_key_length(1);
	refCompletion->set_popup_completion(true);
	refCompletion->set_inline_completion(true);
	refCompletion->set_inline_selection(true);

	std::string path = Glib::getenv("PATH");
#ifdef GLIBMM_REGEX
	Glib::StringArrayHandle dirs = Glib::Regex::split_simple("\\:", path);
	unsigned i=1;
	for ( Glib::Container_Helpers::ArrayHandleIterator<Glib::Container_Helpers::TypeTraits<Glib::ustring> > iter=dirs.begin(); iter!=dirs.end(); iter++ ) {
		try {
			Glib::Dir directory(*iter);
#else
	boost::regex re("\\:");
	boost::sregex_token_iterator dir(path.begin(), path.end(), re, -1);
	boost::sregex_token_iterator end;

	for ( unsigned i=1; dir!=end;) {
		try {
			Glib::Dir directory(*dir++);
#endif /* GLIBMM_REGEX */
			for ( Glib::DirIterator iter=directory.begin(); iter!=directory.end(); iter++ ) {
				Gtk::TreeModel::Row rowCompletion = *(refCompletionModel->append());
				rowCompletion[CompletionColumns.id] = i;
				rowCompletion[CompletionColumns.label] = *iter;
				i++;
			}
		}
		catch (const Glib::FileError& fe) { }
	}

	refCompletion->set_text_column(CompletionColumns.label);

	refCompletion->insert_action_text(_("Browse ..."), 0);
	refCompletion->signal_action_activated().connect(sigc::mem_fun(*this, &ui::on_completion_action_activated));

}

void ui::edit(bool show_pane) {
	std::clog << " * edit" << std::endl;
	/* the row selected */
	Glib::RefPtr<Gtk::TreeSelection> selection = pTreeView->get_selection();
	Gtk::TreeStore::iterator iterTarget = selection->get_selected();
	Gtk::TreeModel::Row row;

	if ( !Glib::thread_supported() ) {
		Glib::thread_init();
	}

	Glib::Thread *tread = Glib::Thread::create(sigc::mem_fun(*this, &ui::getPathFiles), false);

	/* fill the different entries*/
	Glib::ustring type;
	if ( pLabelMode->get_text()=="add" ) {
		if ( !pTableEdit->is_visible() ) {
			pEntryLabel->set_text("");
			pEntryCmd->set_text("");
			pEntryIcon->set_text("");
		}
	}
	else {
		row = *iterTarget;
		type = row[pTreeModel->Columns.type];
		pEntryLabel->set_text(row[pTreeModel->Columns.label]);
		pEntryCmd->set_text(row[pTreeModel->Columns.cmd]);
		pEntryIcon->set_text(row[pTreeModel->Columns.icon]);
	}

	on_entryIcon_changed();

	/* search for the type to select */
	Glib::RefPtr<Gtk::TreeModel> model = pComboBoxType->get_model();
	Gtk::TreeNodeChildren children = model->children();
	Gtk::TreeModel::iterator iter;
	Glib::ustring text = "";

	for ( iter = children.begin(); iter!=children.end() && type!=text; iter++ ) {
		row = *iter;
		row.get_value(0, text);
	}

	if ( pLabelMode->get_text()=="edit" && type==text ) pComboBoxType->set_active(--iter);
	else pComboBoxType->set_active(0);

	pTableDelete->hide();
	if ( show_pane ) {
		pTableEdit->show();
	}
	else {
		if ( !icon_selected.empty() ) {
			pEntryIcon->set_text(icon_selected);
		}
	}

	pWindow->set_focus(*pEntryLabel);
}

void ui::on_entryIcon_changed(void) {
	std::clog << " * on_entryIcon_changed" << std::endl;

	Gtk::Image *pImage;
	_xml->get_widget("imgIcon", pImage);

	Glib::ustring str = pEntryIcon->get_text();
	if ( !str.empty() ) {
		try {
			Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create_from_file(str, 15, 15);
			pImage->set(pixbuf);
		} catch ( Glib::Error ) {
			std::clog << str << " don't exist" << std::endl;
		}
	}
	else {
		pImage->set(GLADE_DIR"/image.png");
		pImage->set_size_request(22, 17);
	}
}

void ui::on_drag_begin(const Glib::RefPtr<Gdk::DragContext>& context) {
	std::clog << " * on_drag_begin" << std::endl;
	_action.clean();
	_action.setType(ROW_MOVED);

	Glib::RefPtr<Gtk::TreeSelection> selection = pTreeView->get_selection();
	Gtk::TreeStore::iterator iterSrc = selection->get_selected();
	Gtk::TreeModel::Path path = pTreeModel->get_path(iterSrc);

	_action.setPathSrc(path);
	std::clog << tab << path.to_string() << std::endl;
}

void ui::on_row_inserted(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter) {
	std::clog << " * on_row_inserted" << std::endl;

	if ( _action.getType()==ROW_MOVED ) {
		if ( _action.getPathDst().empty() ) { // Needed cause children
			std::clog << tab << "pathDst empty" << std::endl;
			_action.setPathDst(path);
			std::clog << tab << path.to_string() << std::endl;
		}
	}
}

void ui::on_drag_end(const Glib::RefPtr<Gdk::DragContext>& context) {
	std::clog << " * on_drag_end" << std::endl;

	if ( !_action.getPathDst().empty() ) {
		std::clog << tab << "not empty" << std::endl;
		std::clog << tab << _action.getPathDst().to_string() << " " << _action.getPathSrc().to_string() << std::endl;
		int iCompare = compare_path(_action.getPathDst(), _action.getPathSrc());
		hl(_action.getPathDst());
		if ( iCompare ) {
			menu_edited(true, true);
		}
	}
	else {
		std::clog << tab << "empty" << std::endl;
		_action.clean();
	}

	hl();
}

static double step = 1;

bool ui::pulse(void) {
	if ( in_progress ) {
		pProgressBar->pulse();
		if ( step==4 ) step = 0;
		pProgressBar->set_fraction((++step)*0.25);
		return true;
	}
	else {
		pWindowForWait->hide();
		step = 1;

		/* resolve a small showing bug */
		pTreeView->expand_all();
		pTreeView->collapse_all();

		return false;
	}
}

void ui::on_btnNew_clicked(void) {
	std::clog << " * on_btnNew_clicked" << std::endl;

	Gtk::Dialog *pDialog;
	_xml->get_widget("dialogNewMenu", pDialog);
	pDialog->set_transient_for(*pWindow);

	int response = pDialog->run();
	pDialog->hide();

	if ( response==Gtk::RESPONSE_OK ) {
		std::clog << tab << "create temporary file" << std::endl;
		char fileTmp[] = "/tmp/fmemenu.XXXXXX";
		if ( mkstemp(fileTmp) ) {
			/* save old menu */
			copy("/tmp/fmemenu.tmp", pMenu->getFilename_c());
			/* save current menu */
			on_btnSave_clicked();
			/* move current menu */
			std::clog << tab << "copy menu in this temporary file" << std::endl;
			copy(fileTmp, pMenu->getFilename_c());

			Gtk::RadioButton *pRadioFgm;
			_xml->get_widget("radiobuttonFgmMenu", pRadioFgm);

			if ( pRadioFgm->get_active() ) {
				if ( !Glib::thread_supported() ) {
					Glib::thread_init();
				}

				_action.clean();
				_action.setType(NEW_MENU);

				_action.setFileOld(fileTmp);
				_action.setFileNew("/tmp/fmemenu.fgm");

				pTreeModel->clear();
	
				Glib::Thread *thread = Glib::Thread::create(sigc::mem_fun(*this, &ui::generate_menu), false);
				pWindowForWait->show();
	
				sigc::connection connex = Glib::signal_timeout().connect(sigc::mem_fun(*this, &ui::pulse), 300);
				menu_edited(true, true);
			}
			else {
				pTreeModel->clear();

				/* restore old menu */
				copy(pMenu->getFilename_c(), "/tmp/fmemenu.tmp");
				/* delete temp old menu */
				unlink("/tmp/fmemenu.tmp");

				std::clog << tab << "copy menu in this temporary file" << std::endl;
				copy(pMenu->getFilename_c(), fileTmp);
				_action.clean();
				_action.setType(NEW_MENU);

				_action.setFileOld(fileTmp);
				_action.setFileNew("none");
				menu_edited(true, true);
			}
		}
		else {
			show_error(_("Can't create temporary file"), _("Not enough disk space ?"));
			exit(EXIT_SUCCESS);
		}
	}
}

void ui::on_btnAdd_clicked(void) {
	std::clog << " * on_btnAdd_clicked" << std::endl;
	pLabelMode->set_text("add");
	edit();
}

void ui::on_btnEdit_clicked(void) {
	std::clog << " * on_btnEdit_clicked" << std::endl;
	pLabelMode->set_text("edit");
	edit();
}

void ui::on_btnDelete_clicked(void) {
	std::clog << " * on_btnDelete_clicked" << std::endl;
	pTableAdd->hide();
	pTableEdit->hide();
	
	if ( preferences.getConfirmForDelete() ) {
		pTableDelete->show();
	}
	else {
		on_btnConfirmDelete_clicked();
	}
}

void ui::on_update_preview(void) {
	std::clog << " * on_update_preview" << std::endl;

	Glib::ustring path = pSelectIcon->get_preview_filename();
	if ( path.empty() ) {
		path = icon_selected;
		icon_selected = "";
	}
	try {
		Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create_from_file(path, 30, 30);
		preview.set(pixbuf);
	} catch ( Glib::Error ) {
		preview.clear();
	}
}

void ui::on_btnIcon_clicked(void) {
	std::clog << " * on_btnIcon_clicked" << std::endl;

	Glib::ustring icon_path = "";

	if ( !pTableEdit->is_visible() ) {
		Glib::RefPtr<Gtk::TreeSelection> selection = pTreeView->get_selection();
		Gtk::TreeStore::iterator iterTarget = selection->get_selected();
		Gtk::TreeModel::Row row = *iterTarget;
		row.get_value(4, icon_path);
		pLabelMode->set_text("edit");
	} else {
		icon_path = pEntryIcon->get_text();
	}

	if ( !icon_path.empty() && icon_path.compare(pSelectIcon->get_filename()) ) {
		pSelectIcon->set_filename(icon_path);
		icon_selected = icon_path;
	} else if ( icon_path.empty() && icon_path.compare(pSelectIcon->get_filename()) ) {
		pSelectIcon->set_filename(Glib::get_current_dir());
	}

	on_update_preview();

	int response = pSelectIcon->run();

	if ( response!=Gtk::RESPONSE_CANCEL ) {
		if ( !pTableEdit->is_visible() ) {
			edit(false);
		}
		icon_selected = pSelectIcon->get_filename();
		if ( !icon_selected.empty() || response!=Gtk::RESPONSE_OK  ) {
			if ( response==Gtk::RESPONSE_OK ) {
				pEntryIcon->set_text(icon_selected);
			}
			else
				pEntryIcon->set_text("");

			if ( !pTableEdit->is_visible() ) {
				if ( response!=Gtk::RESPONSE_OK ) {
					pEntryIcon->set_text("");
				}
				on_btnConfirmEdit_clicked();
			}

			on_entryIcon_changed();
		}
	}

	pSelectIcon->hide();
}

void ui::on_selection_changed(void) {
	std::clog << " * on_selection_changed" << std::endl;

	std::list<Gtk::TreeModel::Path> selected = pIconView->get_selected_items();
	if ( !selected.empty() ) {
		const Gtk::TreeModel::Path& path = *selected.begin();
		Gtk::TreeModel::iterator iter = pListModelIconView->get_iter(path);
		Gtk::TreeModel::Row row = *iter;
		icon_selected = row[IconsColumns.path];
	}
}

void ui::search_file_in_paths(void) {
	std::clog << " * search_file_in_paths" << std::endl;

	in_progress = 1;

	Glib::ustring command = "";

	if ( !pTableEdit->is_visible() ) {
		Glib::RefPtr<Gtk::TreeSelection> selection = pTreeView->get_selection();
		Gtk::TreeStore::iterator iterTarget = selection->get_selected();
		Gtk::TreeModel::Row row = *iterTarget;
		row.get_value(3, command);
		pLabelMode->set_text("edit");
	} else {
		command = pEntryCmd->get_text();
	}

	Glib::ustring command_name = Glib::filename_display_basename(command.substr(0, command.find_first_of(' ')));

	pListModelIconView = Gtk::ListStore::create(IconsColumns);
	pIconView->set_model(pListModelIconView);
	pIconView->set_markup_column(IconsColumns.filename);
	pIconView->set_pixbuf_column(IconsColumns.pixbuf);

	for ( std::vector<Glib::ustring>::iterator iterPath = preferences.getPathsIcons().begin(); iterPath!=preferences.getPathsIcons().end(); iterPath++ ) {
		try {
			Glib::Dir directory(*iterPath);
			for ( Glib::DirIterator iter=directory.begin(); iter!=directory.end(); iter++ ) {
				Glib::ustring icon_name = *iter;
				if ( icon_name.find(command_name)!=Glib::ustring::npos ) {
					Glib::ustring path = *iterPath + "/" + icon_name;
					Gtk::TreeModel::Row row = *(pListModelIconView->append());
					row[IconsColumns.path] = path;
					row[IconsColumns.filename] = Glib::filename_display_basename(icon_name);
					try {
						row[IconsColumns.pixbuf] = Gdk::Pixbuf::create_from_file(path, 30, 30);
					} catch(Glib::Error) {}
				}
			}
		}
		catch (const Glib::FileError& fe) { }
	}

	sleep(1);

	in_progress = 0;
}

void ui::on_btnSearchIcon_clicked(void) {
	std::clog << " * on_btnSearchIcon_clicked" << std::endl;

	if ( !Glib::thread_supported() ) {
		Glib::thread_init();
	}

	Glib::Thread *thread = Glib::Thread::create(sigc::mem_fun(*this, &ui::search_file_in_paths), false);

/* TODO : How to synchronise both thread without blocking the app ? */
//	pWindowForWait->show();
	
//	sigc::connection connex = Glib::signal_timeout().connect(sigc::mem_fun(*this, &ui::pulse), 300);

	int response = pIconViewDialog->run();

	if ( response!=4 ) {
		if ( !pTableEdit->is_visible() ) {
			edit(false);
		}
		if ( !icon_selected.empty() || response!=Gtk::RESPONSE_OK  ) {
			if ( response==Gtk::RESPONSE_OK ) {
				pEntryIcon->set_text(icon_selected);
			} else {
				if ( response!=Gtk::RESPONSE_CANCEL ) {
					pEntryIcon->set_text("");
				}
			}

			if ( !pTableEdit->is_visible() ) {
				if ( response==5 ) {
					pEntryIcon->set_text("");
				}
				on_btnConfirmEdit_clicked();
			}

			on_entryIcon_changed();
		}
	}

	pIconViewDialog->hide();
}

void ui::on_btnUndo_clicked(void) {
	std::clog << " * on_btnUndo_clicked" << std::endl;
	_action = *_iterAction;

	Gtk::TreeModel::Path pathSrc;
	Gtk::TreeModel::Path pathDst;
	Gtk::TreeStore::iterator iterSrc;
	Gtk::TreeStore::iterator iterDst;
	Gtk::TreeModel::Row rowOld;
	Gtk::TreeModel::Row rowNew;
	Gtk::TreeNodeChildren child = pTreeModel->children();
	Gtk::TreeModel::Path newPath;

	int NumberChildren;

	switch ( _action.getType() ) {
		case NEW_MENU:
			std::clog << tab << "NEW_MENU" << std::endl;
			pTreeModel->clear();
			load_menu(_action.getFileOld());
			break;
		case TITLE_CHANGED:
			std::clog << tab << "TITLE_CHANGED" << std::endl;
			pEntryMenuName->set_text(_action.getTitleOld());
			setMenuName(_action.getTitleOld());
			break;
		case ROW_INSERTED:
			std::clog << tab << "ROW_INSERTED" << std::endl;
			pTreeModel->erase(pTreeModel->get_iter(_action.getPathDst()));
			break;
		case ROW_EDITED:
			std::clog << tab << "ROW_EDITED" << std::endl;
			pathSrc = _action.getPathSrc();
			iterSrc = pTreeModel->get_iter(pathSrc);
			rowOld = *iterSrc;
			copy_row(rowOld, _action.getMenuLineOld());
			break;
		case ROW_DELETED:
			std::clog << tab << "ROW_DELETED" << std::endl;
			pathSrc = _action.getPathSrc();
			iterSrc = pTreeModel->get_iter(pathSrc);
			Gtk::TreePath::iterator iter;
			iter = pathSrc.end();
			if ( *(--iter)==0 ) {
				std::clog << tab(2) << "To the begin of a submenu" << std::endl;
				/* To the begin of a submenu */
				if ( pathSrc.size()!=1 ) {
					pathSrc.up();
					rowOld = *(pTreeModel->get_iter(pathSrc));
					child = rowOld.children();
				}
				if ( child.size() ) rowOld = *(pTreeModel->insert(iterSrc));
				else rowOld = *(pTreeModel->append(child));
			}
			else {
				/* To sw in a submenu */
				std::clog << tab(2) << "To sw in a submenu" << std::endl;
				Gtk::TreePath pathTmp = pathSrc;
				if ( pathSrc.size()!=1 ) {
					pathTmp.up();
					rowOld = *(pTreeModel->get_iter(pathTmp));
					child = rowOld.children();
				}
				if ( child.size() <= *(iter) ) rowOld = *(pTreeModel->append(child)); // End of the submenu
				else rowOld = *(pTreeModel->insert(iterSrc));
			}
			copy_row(rowOld, _action.getMenuLineOld());
			NumberChildren = _action.getNumberChildren();
			add_row(rowOld, NumberChildren);
			_iterAction--;
			break;
		case ROW_MOVED:
			std::clog << tab << "ROW_MOVED" << std::endl;
			pathSrc = _action.getPathSrc();
			pathDst = _action.getPathDst();
			iterSrc = pTreeModel->get_iter(pathSrc);
			iterDst = pTreeModel->get_iter(pathDst);
			newPath = pathDst - pathSrc;
			if ( compare_path(pathDst, pathSrc)>0 && (pathSrc.size()==1 || !self_submenu(pathDst, pathSrc) ) ) {
				std::clog << tab(2) << "pathDst > pathSrc : Down, on root" << std::endl;
				/* pathDst > pathSrc : Down, on root */
				/* Dst-- -> Src */
				iterDst = pTreeModel->get_iter(newPath);
				rowNew = *(pTreeModel->insert(iterSrc));
			}
			else {
				/* pathDst < pathSrc */
				if ( pathSrc.size()==1 ) {
					std::clog << tab(2) << "pathSrc.size()==1 : Dst -> Src" << std::endl;
					/* Dst -> Src */
					rowNew = *(pTreeModel->insert_after(iterSrc));
				}
				else {
					Gtk::TreeModel::Path::iterator iter;
					if ( ( pathSrc.size()<pathDst.size() || compare_path(pathDst, pathSrc)<0 ) && ( !self_submenu(pathDst, pathSrc) || (pathSrc.size()-pathDst.size())==1 )) {
						std::clog << tab(2) << "pathSrc.size()!=1 : Dst -> Src++" << std::endl;
						/* Dst -> Src++ */
						newPath = pathDst + pathSrc;
						iterSrc = pTreeModel->get_iter(newPath);
						iter = newPath.end();
					}
					else {
						std::clog << tab(2) << "pathSrc.size()!=1: Dst -> Src" << std::endl;
						/* Dst -> Src */
						newPath = pathSrc;
						iter = newPath.end();
					}
					if ( *(--iter)==0 ) {
						std::clog << tab(2) << "pathSrc.size()!=1 : To the begin of a submenu" << std::endl;
						/* To the begin of a submenu */
						newPath.up();
						rowNew = *(pTreeModel->get_iter(newPath));
						child = rowNew.children();
						if ( child.size() ) rowNew = *(pTreeModel->insert(iterSrc));
						else rowNew = *(pTreeModel->append(child));
					}
					else {
						std::clog << tab(2) << "pathSrc.size()!=1 : Sw in a submenu" << std::endl;
						/* To sw in a submenu */
						pathSrc = newPath;
						pathSrc.up();
						rowNew = *(pTreeModel->get_iter(pathSrc));
						child = rowNew.children();
						if ( child.size() <= *(iter) ) rowNew = *(pTreeModel->append(child)); // End of the submenu
						else {
							iterSrc = pTreeModel->get_iter(newPath);
							rowNew = *(pTreeModel->insert(iterSrc));
						}
					}
				}
			}
			rowOld = *iterDst;
			copy_row(rowNew, rowOld);
			pTreeModel->erase(iterDst);
			break;
	}

	if ( newPath.size() ) {
		pTreeView->scroll_to_row(newPath);
		pTreeView->expand_to_path(newPath);
		hl(newPath);
	}

	_iterAction++;
	pBtnRedo->set_sensitive(true);
	if ( _iterAction==_listAction.end() ) {
		pBtnUndo->set_sensitive(false);
	}
	menu_edited(true);

	hl();
}

void ui::on_btnRedo_clicked(void) {
	std::clog << " * on_btnRedo_clicked" << std::endl;
	_iterAction--;
	_action = *_iterAction;

	Gtk::TreeModel::Path pathSrc;
	Gtk::TreeModel::Path pathDst;
	Gtk::TreeStore::iterator iterSrc;
	Gtk::TreeStore::iterator iterDst;
	Gtk::TreeModel::Row rowOld;
	Gtk::TreeModel::Row rowNew;
	Gtk::TreeNodeChildren child = pTreeModel->children();

	switch ( _action.getType() ) {
		case NEW_MENU:
			std::clog << tab << "NEW_MENU" << std::endl;
			pTreeModel->clear();
			load_menu(_action.getFileNew());
			break;
		case TITLE_CHANGED:
			std::clog << tab << "TITLE_CHANGED" << std::endl;
			pEntryMenuName->set_text(_action.getTitleNew());
			setMenuName(_action.getTitleOld());
			break;
		case ROW_INSERTED:
			std::clog << tab << "ROW_INSERTED" << std::endl;
			pathDst = _action.getPathDst();
			iterDst = pTreeModel->get_iter(pathDst);
			Gtk::TreePath::iterator iter;
			iter = pathDst.end();
			if ( *(--iter)==0 ) {
				std::clog << tab(2) << "To the begin of a submenu" << std::endl;
				/* To the begin of a submenu */
				if ( pathDst.size()!=1 ) {
					pathDst.up();
					rowNew = *(pTreeModel->get_iter(pathDst));
					child = rowNew.children();
				}
				if ( child.size() ) rowNew = *(pTreeModel->insert(iterDst));
				else rowNew = *(pTreeModel->append(child));
			}
			else {
				/* To sw in a submenu */
				std::clog << tab(2) << "To sw in a submenu" << std::endl;
				Gtk::TreePath pathTmp = pathDst;
				if ( pathDst.size()!=1 ) {
					pathTmp.up();
					rowNew = *(pTreeModel->get_iter(pathTmp));
					child = rowNew.children();
				}
				if ( child.size() <= *(iter) ) rowNew = *(pTreeModel->append(child)); // End of the submenu
				else rowNew = *(pTreeModel->insert(iterSrc));
			}
			copy_row(rowNew, _action.getMenuLineNew());
			break;
		case ROW_EDITED:
			std::clog << tab << "ROW_EDITED" << std::endl;
			pathSrc = _action.getPathSrc();
			iterSrc = pTreeModel->get_iter(pathSrc);
			rowNew = *iterSrc;
			copy_row(rowNew, _action.getMenuLineNew());
			break;
		case ROW_DELETED:
			std::clog << tab << "ROW_DELETED" << std::endl;
			for ( int i = _action.getNumberChildren(); i; i--, _iterAction--);
			pTreeModel->erase(pTreeModel->get_iter((*_iterAction).getPathSrc()));
			break;
		case ROW_MOVED:
			std::clog << tab << "ROW_MOVED" << std::endl;
			pathSrc = _action.getPathSrc();
			pathDst = _action.getPathDst();
			iterSrc = pTreeModel->get_iter(pathSrc);
			iterDst = pTreeModel->get_iter(pathDst);
			Gtk::TreePath newPath = pathDst - pathSrc;
			if ( compare_path(pathDst, pathSrc)>0 && pathDst.size()==1 && !self_submenu(pathDst, pathSrc)) {
				std::clog << tab(2) << "pathDst > pathSrc : Down, on root" << std::endl;
				/* pathDst > pathSrc : Down, on root */
				/* Src -> Dst-- */
				newPath = pathSrc - pathDst;
				iterDst = pTreeModel->get_iter(newPath);
				rowNew = *(pTreeModel->insert_after(iterDst));
			}
			else {
				/* pathDst < pathSrc */
				if ( pathDst.size()==1 ) {
					std::clog << tab(2) << "Src -> Dst" << std::endl;
					/* Src -> Dst */
					rowNew = *(pTreeModel->insert(iterDst));
				}
				else {
					/* Dst -> Src++ */
					Gtk::TreePath::iterator iter = pathDst.end();
					if ( *(--iter)==0 ) {
						std::clog << tab(2) << "To the begin of a submenu" << std::endl;
						/* To the begin of a submenu */
						pathDst.up();
						rowNew = *(pTreeModel->get_iter(pathDst));
						child = rowNew.children();
						if ( child.size() ) rowNew = *(pTreeModel->insert(iterDst));
						else rowNew = *(pTreeModel->append(child));
					}
					else {
						std::clog << tab(2) << "To sw in a submenu" << std::endl;
						/* To sw in a submenu */
						newPath = pathDst;
						newPath.up();
						rowNew = *(pTreeModel->get_iter(newPath));
						child = rowNew.children();
						if ( child.size() <= *(iter) ) rowNew = *(pTreeModel->append(child)); // End of the submenu
						else {
							iterDst = pTreeModel->get_iter(pathDst);
							rowNew = *(pTreeModel->insert(iterDst));
						}
					}
				}
			}
			rowOld = *iterSrc;
			copy_row(rowNew, rowOld);
			pTreeModel->erase(iterSrc);
			break;
	}

	if ( pathDst.size() ) {
		pTreeView->scroll_to_row(pathDst);
		pTreeView->expand_to_path(pathDst);
		hl(pathDst);
	}

	pBtnUndo->set_sensitive(true);

	if ( _iterAction==_listAction.begin() ) {
		pBtnRedo->set_sensitive(false);
	}
	menu_edited(true);

	hl();
}

void ui::on_btnPref_clicked(void) {
	std::clog << " * on_btnPref_clicked" << std::endl;

	Gtk::Dialog *pDialog = NULL;
	_xml->get_widget("dialogPref", pDialog);
	pDialog->set_transient_for(*pWindow);

	Gtk::HBox *pHBoxEncoding = NULL;
	_xml->get_widget("hboxEncoding", pHBoxEncoding);

	if ( isFluxboxGt10rc2() )
		pHBoxEncoding->show();
	else
		pHBoxEncoding->hide();

	Gtk::CheckButton *pCB;
	_xml->get_widget("checkbuttonConfirmDelete", pCB);
	pCB->set_active(!preferences.getConfirmForDelete());

	Gtk::ComboBox *pComboBoxEncoding;
	_xml->get_widget("comboboxEncoding", pComboBoxEncoding);

	/* search for the type to select */
	const Glib::ustring encoding = pMenu->getEncoding();

	Glib::RefPtr<Gtk::TreeModel> model = pComboBoxEncoding->get_model();
	Gtk::TreeNodeChildren children = model->children();
	Gtk::TreeModel::iterator iter;
	Glib::ustring text = "";
	Gtk::TreeModel::Row row;

	for ( iter = children.begin(); iter!=children.end() && encoding!=text; iter++ ) {
		row = *iter;
		row.get_value(0, text);
	}

	if ( encoding==text ) pComboBoxEncoding->set_active(--iter);
	else pComboBoxEncoding->set_active(0);

	if ( pDialog->run()==Gtk::RESPONSE_OK ) {
		preferences.setConfirmForDelete(!pCB->get_active());
	
		row = *(pComboBoxEncoding->get_active());

		row.get_value(0, text);
		pMenu->setEncoding(text);

		if ( encoding!=text )
			menu_edited(false, true);
	}

	pDialog->hide();
}

void ui::on_btnSave_clicked() {
	std::clog << " * on_btnSave_clicked" << std::endl;
	int iEnfant=0;

	Gtk::TreeModel::Children children = pTreeModel->children();

	/* TODO : save the old menu */

	std::ofstream menu(pMenu->getFilename_c());

	menu << "#" << std::endl << "# Generated by Fluxbox Menu Editor ( fme )" << std::endl << "#" << std::endl << std::endl;

	menu << "[begin]";
	std::clog << "[begin]";
	const Glib::ustring& ustr = getMenuName();
	if ( ustr.size() ) {
		/* _FLUXBOX_GT_1_0_RC2_ */
		try {
			/* XXX : To be honest, I don't know why it works... */
			if ( isFluxboxGt10rc2() && Glib::get_charset() ) { // if utf-8
				menu << " (" << ustr << ")" << std::endl;
			}
			else {
				if ( isFluxboxGt10rc2() )
					menu << " (" << Glib::convert(ustr, "ISO-8859-15", "UTF-8") << ")" << std::endl;
				else
					menu << " (" << ustr << ")" << std::endl;
			}
		} catch ( Glib::ConvertError ) {
			std::clog << " !! ConvertError" << std::endl;
			menu << " (" << ustr << ")" << std::endl;
		}
		/* _FLUXBOX_GT_1_0_RC2_ */
		std::clog << " (" << ustr << ")" << std::endl;
	}
	else menu << std::endl;

	/* _FLUXBOX_GT_1_0_RC2_ */
	if ( isFluxboxGt10rc2() ) {
		menu << "[encoding] {" << pMenu->getEncoding() << "}" << std::endl;
	}
	/* _FLUXBOX_GT_1_0_RC2_ */

	std::clog << " * save_submenu" << std::endl;
	save_submenu(menu, iEnfant, children, "menu");

	/* _FLUXBOX_GT_1_0_RC2_ */
	if ( isFluxboxGt10rc2() ) {
		menu << "[endencoding]" << std::endl;
	}
	/* _FLUXBOX_GT_1_0_RC2_ */

	menu << "[end]" << std::endl;
	std::clog << "[end]" << std::endl;

	menu.close();

	menu_edited(false);

	pBtnSave->set_sensitive(false);
}

void ui::on_btnAbout_clicked(void) {
	Gtk::AboutDialog *pDialog;
	_xml->get_widget("dialogAbout", pDialog);

	pDialog->set_transient_for(*pWindow);
	pDialog->set_name(PACKAGE_NAME);
	pDialog->set_version(PACKAGE_VERSION);
	pDialog->run();
	pDialog->hide();
}

void ui::on_btnQuit_clicked(void) {
	if ( confirm_exit() ) {
		Gtk::Main::quit();
	}
}

void ui::on_btnConfirmAdd_clicked(void) {
	std::clog << " * on_btnConfirmAdd_clicked" << std::endl;

	pTableAdd->hide();

	MenuLine lineNew;
	Glib::RefPtr<Gtk::TreeSelection> selection = pTreeView->get_selection();
	Gtk::TreeStore::iterator iterTarget = selection->get_selected();
	Gtk::TreeStore::iterator iter;
	Gtk::TreeModel::Row row;
	Gtk::TreeModel::Path pathDst;

	Gtk::TreeModel::Row rowCombo = *(pComboBoxWhere->get_active());
	Glib::ustring where;
	rowCombo.get_value(0, where);

	if ( iterTarget ) {
		if ( where==_("before") ) {
			std::clog << tab << "before" << std::endl;
			iter = pTreeModel->insert(iterTarget);
		}
		else {
			if ( where==_("in") ) {
				std::clog << tab << "in" << std::endl;
				row = *iterTarget;
				iter = pTreeModel->append(row->children());
			}
			else {
				std::clog << tab << "after" << std::endl;
				iter = pTreeModel->insert_after(iterTarget);
			}
		}
	}
	else {
		iter = pTreeModel->append();
	}

	_action.clean();
	_action.setType(ROW_INSERTED);

	row = *iter;
	pathDst = pTreeModel->get_path(iter);
	rowCombo = *(pComboBoxType->get_active());

	rowCombo.get_value(0, lineNew.type);
	lineNew.label = pEntryLabel->get_text();
	lineNew.cmd = pEntryCmd->get_text();
	lineNew.icon = pEntryIcon->get_text();

	copy_row(row, lineNew);
	_action.setMenuLineNew(lineNew);
	_action.setPathDst(pathDst);
	menu_edited(true, true);
}

void ui::on_btnHideAdd_clicked(void) {
	std::clog << " * on_btnHideAdd_clicked" << std::endl;
	pTableAdd->hide();
}

void ui::on_btnConfirmEdit_clicked(void) {
	std::clog << " * on_btnConfirmEdit_clicked" << std::endl;
	
	if ( pLabelMode->get_text()=="edit" ) {
		MenuLine lineOld, lineNew;
		Glib::RefPtr<Gtk::TreeSelection> selection = pTreeView->get_selection();
		Gtk::TreeStore::iterator iterTarget = selection->get_selected();
		Gtk::TreeModel::Row row = *iterTarget;
	
		_action.clean();
		_action.setType(ROW_EDITED);
	
		row = *iterTarget;
		copy_row(lineOld, row);
		_action.setMenuLineOld(lineOld);
	
		_action.setPathSrc(pTreeModel->get_path(iterTarget));
	
		Gtk::TreeModel::iterator iter = pComboBoxType->get_active();
		Gtk::TreeModel::Row rowCombo = *iter;
	
		rowCombo.get_value(0, lineNew.type);
		lineNew.label = pEntryLabel->get_text();
		lineNew.cmd = pEntryCmd->get_text();
		lineNew.icon = pEntryIcon->get_text();

		if ( lineOld!=lineNew ) {
			copy_row(row, lineNew);
			_action.setMenuLineNew(lineNew);
			menu_edited(true, true);
		}
	}
	else {
		show_tableAdd();
	}

	pTableEdit->hide();
}

void ui::on_btnHideEdit_clicked(void) {
	std::clog << " * on_btnHideEdit_clicked" << std::endl;
	pTableEdit->hide();
}

void ui::on_btnConfirmDelete_clicked(void) {
	std::clog << " * on_btnConfirmDelete_clicked" << std::endl;
	MenuLine line;
	Glib::RefPtr<Gtk::TreeSelection> selection = pTreeView->get_selection();
	Gtk::TreeStore::iterator iterTarget = selection->get_selected();
	Gtk::TreeModel::Row row = *iterTarget;

	int NumberChildren = getNumberChildren(iterTarget);
	int Numbertmp = NumberChildren;
	remove_row(iterTarget, Numbertmp);

	_action.clean();
	_action.setType(ROW_DELETED);

	row = *iterTarget;
	copy_row(line, row);
	_action.setMenuLineOld(line);

	_action.setPathSrc(pTreeModel->get_path(iterTarget));
	_action.setNumberChildren(NumberChildren);
	pTreeModel->erase(iterTarget);
	menu_edited(true, true);

	on_btnHideDelete_clicked();
}

void ui::on_btnHideDelete_clicked(void) {
	std::clog << " * on_btnHideDelete_clicked" << std::endl;
	pTableDelete->hide();
	hl();
}

void ui::on_btnAddPathIcons_clicked(void) {
	std::clog << " * on_btnAddPathIcons_clicked" << std::endl;

	Glib::ustring path_icons = pSelectPathIcons->get_filename();

	std::vector<Glib::ustring> &paths_icons = preferences.getPathsIcons();
	std::vector<Glib::ustring>::iterator iterPath = paths_icons.begin();
	
	for ( ; iterPath!=paths_icons.end() && path_icons!=*iterPath; iterPath++ );

	if ( iterPath==paths_icons.end() ) {
		Gtk::TreeModel::Row row;
		Gtk::ListStore::iterator iter;

		iter = pListModelPathsIcons->append();
		row = *iter;
		row[pListModelPathsIcons->Columns.path] = path_icons;
		paths_icons.push_back(path_icons);
	}
}

void ui::on_btnDelPathIcons_clicked(void) {
	std::clog << " * on_btnDelPathIcons_clicked" << std::endl;

	Glib::RefPtr<Gtk::TreeSelection> selection = pTreeViewPathsIcons->get_selection();
	if ( selection->count_selected_rows()!=0 ) {
		Gtk::TreeStore::iterator iterTarget = selection->get_selected();
		Gtk::TreeModel::Row row = *iterTarget;
		Glib::ustring path_icons = row[pListModelPathsIcons->Columns.path];
		pListModelPathsIcons->erase(iterTarget);

		std::vector<Glib::ustring> &paths_icons = preferences.getPathsIcons();
		std::vector<Glib::ustring>::iterator iterPath = paths_icons.begin();
		for ( ; iterPath!=paths_icons.end() && path_icons!=*iterPath; iterPath++ );
		paths_icons.erase(iterPath);
	}
}
