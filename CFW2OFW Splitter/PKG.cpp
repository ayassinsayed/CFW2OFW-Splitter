#include "PKG.h"


PKG::PKG(const QString &path) : path(path) {
}


PKG::~PKG() {
	//QDir().remove(package_conf);
}


bool PKG::generate_debug_package() {
	QByteArray title_id, part_number, version, app_ver, cat, contentid, klicensee, drmtype, contenttype, packagetype, packageversion;
	QString packagename;
	if (path.isEmpty())
		return false;
	if (path.endsWith('\\'))
		path.remove(path.size() - 1, 1);
	title_id = path.mid(1 + path.lastIndexOf('\\')).toLocal8Bit();
	if (!title_id.startsWith("BL"))
		if (!title_id.startsWith("BC"))
			if (!title_id.startsWith("NP"))
				if (!title_id.startsWith("SL"))
					return false;
	if (title_id.contains('_')) {
		title_id = title_id.left(title_id.lastIndexOf('_'));
		part_number = ".part" + title_id.mid(1 + title_id.lastIndexOf('_'));
	}
	PARAM p(path + "\\PARAM.SFO");
	if (!p.isparam())
		return false;
	version = p.at(PARAM::VERSION);
	if (version.isEmpty()) {
		version = "01.00";
		p.insert(PARAM::VERSION, version);
	}
	cat = p.at(PARAM::CATEGORY);
	if (cat.isEmpty())
		return false;
	if (cat == "DG" || cat == "HD" || cat == "HG" || cat == "AP" || cat == "AM" || cat == "AV" || cat == "AT" || cat == "CB" || cat == "AS" || cat == "HM" || cat == "SF") {
		app_ver = "01.00";
		p.insert(PARAM::APP_VER, app_ver);
		p.insert(PARAM::TARGET_APP_VER, "00.01");
		contentid = EBOOT(path + "\\USRDIR\\EBOOT.BIN").contentid();
		klicensee = "0x00000000000000000000000000000000";
		drmtype = "Free";
		contenttype = "GameExec";
		packagetype = "HDDGamePatch";
		packageversion = version;
		packagename = contentid + "-A" + app_ver.remove(2, 1) + "-V" + version.remove(2, 1);
	}
	else if (cat == "GD") {
		app_ver = p.at(PARAM::APP_VER);
		p.insert(PARAM::TARGET_APP_VER, "00.01");
		contentid = EBOOT(path + "\\USRDIR\\EBOOT.BIN").contentid();
		klicensee = "0x00000000000000000000000000000000";
		drmtype = "Free";
		contenttype = "GameData";
		packagetype = "DiscGamePatch";
		packageversion = version;
		packagename = contentid + "-A" + app_ver.remove(2, 1) + "-V" + version.remove(2, 1);
	}
	else if (cat == "1P" || cat == "MN" || cat == "PE" || cat == "2P" || cat == "2G" || cat == "2D" || cat == "PP" || cat == "WT") {
		app_ver = "01.00";
		contentid = EDAT(path + "\\USRDIR\\ISO.BIN.EDAT").contentid();
		klicensee = "0x72F990788F9CFF745725F08E4C128387";
		drmtype = "Local";
		if (cat == "1P")
			contenttype = "PS1Game";
		else if (cat == "2G" || cat == "2P" || cat == "2D")
			contenttype = "PS2Game";
		else if (cat == "MN")
			contenttype = "minis";
		else if (cat == "PE")
			contenttype = "PSPemu";
		else if (cat == "PP")
			contenttype = "PSPData";
		else if (cat == "WT")
			contenttype = "XMBLink";
		else
			contenttype = "GameData";
		packageversion = version;
		packagename = contentid;
	}
	else
		return false;
	if (contentid.isEmpty())
		return false;
	QFile f(package_conf);
	if (!f.open(QIODevice::WriteOnly))
		return false;
	QTextStream out(&f);
	out << "ContentID = " << contentid << endl
		<< "Klicensee = " << klicensee << endl
		<< "DRMType = " << drmtype << endl
		<< "InstallDirectory = " << title_id << endl
		<< "ContentType = " << contenttype << endl
		<< "PackageType = " << packagetype << endl
		<< "PackageVersion = " << packageversion << endl;
	if (!p.close())
		return false;
	f.close();
	QProcess proc;
	proc.setProcessChannelMode(QProcess::ForwardedChannels);
	proc.start(psn_package_npdrm, QStringList() << "-n" << "-f" << package_conf << path);
	if (!proc.waitForStarted())
		return false;
	if (!proc.waitForFinished(-1))
		return false;
	if (proc.exitCode() != QProcess::NormalExit && proc.exitStatus() != QProcess::NormalExit)
		return false;
	QString packagepath = QDir::currentPath() + '\\';
	if (!QDir().rename(packagepath + packagename + ".pkg", packagepath + packagename + part_number + ".pkg"))
		return false;
	return true;
}
