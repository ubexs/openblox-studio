/*
 * Copyright (C) 2017 John M. Harris, Jr. <johnmh@openblox.org>
 *
 * This file is part of OpenBlox Studio.
 *
 * OpenBlox Studio is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenBlox Studio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with OpenBlox Studio.	 If not, see <https://www.gnu.org/licenses/>.
 */

#include <openblox.h>

#include <QApplication>
#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QCommandLineParser>
#include <QThread>
#include <QStringListModel>

#include "StudioWindow.h"

void defaultValues(QSettings* settings){
	settings->setValue("first_run", false);
	settings->setValue("dark_theme", true);
}

int main(int argc, char** argv){
	QApplication app(argc, argv);

	OB::Studio::StudioWindow::pathToStudioExecutable = std::string(argv[0]);

	app.setApplicationName("OpenBlox Studio");
	app.setApplicationVersion("0.1.1");
	app.setOrganizationDomain("openblox.org");
	app.setOrganizationName("OpenBlox");

	#ifdef _WIN32
	QSettings settings("OpenBlox", "OpenBloxStudio");
	#elif defined(__linux)
	QSettings settings("openblox-studio", "openblox-studio");
	#else
	QSettings settings;
	#endif
	bool firstRun = settings.value("first_run", true).toBool();
	if(firstRun){
		defaultValues(&settings);
	}

	#define DARK_THEME_DEFAULT true
	#ifndef WIN32
	#undef DARK_THEME_DEFAULT
	#define DARK_THEME_DEFAULT false
	#endif
	bool useDarkTheme = settings.value("dark_theme", DARK_THEME_DEFAULT).toBool();

	if(useDarkTheme){
		QFile f(":qdarkstyle/style.qss");
		if(f.exists()){
			f.open(QFile::ReadOnly | QFile::Text);
			QTextStream ts(&f);
			app.setStyleSheet(ts.readAll());
		}
	}

	QCommandLineParser parser;
	parser.setApplicationDescription("OpenBlox Studio");
	parser.addHelpOption();
	parser.addVersionOption();

	QCommandLineOption newOpt("new", "Starts a new game instance on initialization.");
	parser.addOption(newOpt);
	
	parser.process(app);

    OB::OBEngine* eng = new OB::OBEngine();

	OB::Studio::StudioWindow* win = new OB::Studio::StudioWindow();

	#ifdef WIN32
	settings.beginGroup("main_window");
	{
		if(settings.contains("geometry")){
			win->restoreGeometry(settings.value("geometry").toByteArray());
		}
		if(settings.contains("state")){
			win->restoreState(settings.value("state").toByteArray());
		}
	}
	settings.endGroup();
	#endif

	win->show();

	QComboBox* cmdBar = win->cmdBar;
	settings.beginGroup("command_history");
	{
		if(settings.contains("max_history")){
			cmdBar->setMaxCount(settings.value("max_history").toInt());
		}
		if(settings.contains("history")){
			cmdBar->addItems(settings.value("history").toStringList());
			cmdBar->setCurrentIndex(cmdBar->count());
			cmdBar->setCurrentText("");
		}
	}
	settings.endGroup();

	if(parser.isSet(newOpt)){
		win->newInstance();
	}

	while(win->isVisible()){
		app.processEvents();
		QThread::msleep(10);
	}

	#ifdef WIN32
	settings.beginGroup("main_window");
	{
		settings.setValue("geometry", win->saveGeometry());
		settings.setValue("state", win->saveState());
	}
	settings.endGroup();
	#endif

	settings.beginGroup("command_history");
	{
	    settings.setValue("history", ((QStringListModel*)(cmdBar->model()))->stringList());
	}
	settings.endGroup();

	return 0;
}


