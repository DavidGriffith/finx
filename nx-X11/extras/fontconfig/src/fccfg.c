/*
 * $RCSId: xc/lib/fontconfig/src/fccfg.c,v 1.23 2002/08/31 22:17:32 keithp Exp $
 *
 * Copyright © 2000 Keith Packard
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Keith Packard not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Keith Packard makes no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * KEITH PACKARD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL KEITH PACKARD BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include "fcint.h"

#if defined (_WIN32) && (defined (PIC) || defined (DLL_EXPORT))
#define STRICT
#include <windows.h>
#undef STRICT
#endif

#if defined (_WIN32) && !defined (R_OK)
#define R_OK 4
#endif

FcConfig    *_fcConfig;

FcConfig *
FcConfigCreate (void)
{
    FcSetName	set;
    FcConfig	*config;

    config = malloc (sizeof (FcConfig));
    if (!config)
	goto bail0;
    FcMemAlloc (FC_MEM_CONFIG, sizeof (FcConfig));
    
    config->configDirs = FcStrSetCreate ();
    if (!config->configDirs)
	goto bail1;
    
    config->configFiles = FcStrSetCreate ();
    if (!config->configFiles)
	goto bail2;
    
    config->fontDirs = FcStrSetCreate ();
    if (!config->fontDirs)
	goto bail3;
    
    config->acceptGlobs = FcStrSetCreate ();
    if (!config->acceptGlobs)
	goto bail4;

    config->rejectGlobs = FcStrSetCreate ();
    if (!config->rejectGlobs)
	goto bail5;

    config->acceptPatterns = FcFontSetCreate ();
    if (!config->acceptPatterns)
	goto bail6;
    
    config->rejectPatterns = FcFontSetCreate ();
    if (!config->rejectPatterns)
	goto bail7;

    config->cache = 0;
    if (FcConfigHome())
	if (!FcConfigSetCache (config, (FcChar8 *) ("~/" FC_USER_CACHE_FILE)))
	    goto bail8;

#ifdef _WIN32
    if (config->cache == 0)
    {
	/* If no home, use the temp folder. */
	FcChar8	    dummy[1];
	int	    templen = GetTempPath (1, dummy);
	FcChar8     *temp = malloc (templen + 1);

	if (temp)
	{
	    FcChar8 *cache_dir;

	    GetTempPath (templen + 1, temp);
	    cache_dir = FcStrPlus (temp, FC_USER_CACHE_FILE);
	    free (temp);
	    if (!FcConfigSetCache (config, cache_dir))
	    {
		FcStrFree (cache_dir);
		goto bail6;
	    }
	    FcStrFree (cache_dir);
	}
    }
#endif

    config->blanks = 0;

    config->substPattern = 0;
    config->substFont = 0;
    config->maxObjects = 0;
    for (set = FcSetSystem; set <= FcSetApplication; set++)
	config->fonts[set] = 0;

    config->rescanTime = time(0);
    config->rescanInterval = 30;    
    
    return config;

bail8:
    FcFontSetDestroy (config->rejectPatterns);
bail7:
    FcFontSetDestroy (config->acceptPatterns);
bail6:
    FcStrSetDestroy (config->rejectGlobs);
bail5:
    FcStrSetDestroy (config->acceptGlobs);
bail4:
    FcStrSetDestroy (config->fontDirs);
bail3:
    FcStrSetDestroy (config->configFiles);
bail2:
    FcStrSetDestroy (config->configDirs);
bail1:
    free (config);
    FcMemFree (FC_MEM_CONFIG, sizeof (FcConfig));
bail0:
    return 0;
}

typedef struct _FcFileTime {
    time_t  time;
    FcBool  set;
} FcFileTime;

static FcFileTime
FcConfigNewestFile (FcStrSet *files)
{
    FcStrList	    *list = FcStrListCreate (files);
    FcFileTime	    newest = { 0, FcFalse };
    FcChar8	    *file;
    struct  stat    statb;

    if (list)
    {
	while ((file = FcStrListNext (list)))
	    if (stat ((char *) file, &statb) == 0)
		if (!newest.set || statb.st_mtime - newest.time > 0)
		{
		    newest.set = FcTrue;
		    newest.time = statb.st_mtime;
		}
	FcStrListDone (list);
    }
    return newest;
}

FcBool
FcConfigUptoDate (FcConfig *config)
{
    FcFileTime	config_time, font_time;
    time_t	now = time(0);
    if (!config)
    {
	config = FcConfigGetCurrent ();
	if (!config)
	    return FcFalse;
    }
    config_time = FcConfigNewestFile (config->configFiles);
    font_time = FcConfigNewestFile (config->fontDirs);
    if ((config_time.set && config_time.time - config->rescanTime > 0) ||
	(font_time.set && (font_time.time - config->rescanTime) > 0))
    {
	return FcFalse;
    }
    config->rescanTime = now;
    return FcTrue;
}

static void
FcSubstDestroy (FcSubst *s)
{
    FcSubst *n;
    
    while (s)
    {
	n = s->next;
	if (s->test)
	    FcTestDestroy (s->test);
	if (s->edit)
	    FcEditDestroy (s->edit);
	free (s);
	FcMemFree (FC_MEM_SUBST, sizeof (FcSubst));
	s = n;
    }
}

void
FcConfigDestroy (FcConfig *config)
{
    FcSetName	set;

    if (config == _fcConfig)
	_fcConfig = 0;

    FcStrSetDestroy (config->configDirs);
    FcStrSetDestroy (config->fontDirs);
    FcStrSetDestroy (config->configFiles);
    FcStrSetDestroy (config->acceptGlobs);
    FcStrSetDestroy (config->rejectGlobs);
    FcFontSetDestroy (config->acceptPatterns);
    FcFontSetDestroy (config->rejectPatterns);

    if (config->blanks)
	FcBlanksDestroy (config->blanks);

    if (config->cache)
	FcStrFree (config->cache);

    FcSubstDestroy (config->substPattern);
    FcSubstDestroy (config->substFont);
    for (set = FcSetSystem; set <= FcSetApplication; set++)
	if (config->fonts[set])
	    FcFontSetDestroy (config->fonts[set]);

    free (config);
    FcMemFree (FC_MEM_CONFIG, sizeof (FcConfig));
}

/*
 * Scan the current list of directories in the configuration
 * and build the set of available fonts. Update the
 * per-user cache file to reflect the new configuration
 */

FcBool
FcConfigBuildFonts (FcConfig *config)
{
    FcFontSet	    *fonts;
    FcGlobalCache   *cache;
    FcStrList	    *list;
    FcChar8	    *dir;

    fonts = FcFontSetCreate ();
    if (!fonts)
	goto bail0;
    
    cache = FcGlobalCacheCreate ();
    if (!cache)
	goto bail1;

    if (config->cache)
	FcGlobalCacheLoad (cache, config->cache);

    list = FcConfigGetFontDirs (config);
    if (!list)
	goto bail1;

    while ((dir = FcStrListNext (list)))
    {
	if (FcDebug () & FC_DBG_FONTSET)
	    printf ("scan dir %s\n", dir);
	FcDirScanConfig (fonts, config->fontDirs, cache, 
			 config->blanks, dir, FcFalse, config);
    }
    
    FcStrListDone (list);
    
    if (FcDebug () & FC_DBG_FONTSET)
	FcFontSetPrint (fonts);

    if (config->cache)
	FcGlobalCacheSave (cache, config->cache);
    FcGlobalCacheDestroy (cache);

    FcConfigSetFonts (config, fonts, FcSetSystem);
    
    return FcTrue;
bail1:
    FcFontSetDestroy (fonts);
bail0:
    return FcFalse;
}

FcBool
FcConfigSetCurrent (FcConfig *config)
{
    if (!config->fonts)
	if (!FcConfigBuildFonts (config))
	    return FcFalse;

    if (_fcConfig)
	FcConfigDestroy (_fcConfig);
    _fcConfig = config;
    return FcTrue;
}

FcConfig *
FcConfigGetCurrent (void)
{
    if (!_fcConfig)
	if (!FcInit ())
	    return 0;
    return _fcConfig;
}

FcBool
FcConfigAddConfigDir (FcConfig	    *config,
		      const FcChar8 *d)
{
    return FcStrSetAddFilename (config->configDirs, d);
}

FcStrList *
FcConfigGetConfigDirs (FcConfig   *config)
{
    if (!config)
    {
	config = FcConfigGetCurrent ();
	if (!config)
	    return 0;
    }
    return FcStrListCreate (config->configDirs);
}

FcBool
FcConfigAddFontDir (FcConfig	    *config,
		    const FcChar8   *d)
{
    return FcStrSetAddFilename (config->fontDirs, d);
}

FcBool
FcConfigAddDir (FcConfig	    *config,
		const FcChar8	    *d)
{
    return (FcConfigAddConfigDir (config, d) && 
	    FcConfigAddFontDir (config, d));
}

FcStrList *
FcConfigGetFontDirs (FcConfig	*config)
{
    if (!config)
    {
	config = FcConfigGetCurrent ();
	if (!config)
	    return 0;
    }
    return FcStrListCreate (config->fontDirs);
}

FcBool
FcConfigAddConfigFile (FcConfig	    *config,
		       const FcChar8   *f)
{
    FcBool	ret;
    FcChar8	*file = FcConfigFilename (f);
    
    if (!file)
	return FcFalse;
    
    ret = FcStrSetAdd (config->configFiles, file);
    FcStrFree (file);
    return ret;
}

FcStrList *
FcConfigGetConfigFiles (FcConfig    *config)
{
    if (!config)
    {
	config = FcConfigGetCurrent ();
	if (!config)
	    return 0;
    }
    return FcStrListCreate (config->configFiles);
}

FcBool
FcConfigSetCache (FcConfig	*config,
		  const FcChar8	*c)
{
    FcChar8    *new = FcStrCopyFilename (c);
    
    if (!new)
	return FcFalse;
    if (config->cache)
	FcStrFree (config->cache);
    config->cache = new;
    return FcTrue;
}

FcChar8 *
FcConfigGetCache (FcConfig  *config)
{
    if (!config)
    {
	config = FcConfigGetCurrent ();
	if (!config)
	    return 0;
    }
    return config->cache;
}

FcFontSet *
FcConfigGetFonts (FcConfig	*config,
		  FcSetName	set)
{
    if (!config)
    {
	config = FcConfigGetCurrent ();
	if (!config)
	    return 0;
    }
    return config->fonts[set];
}

void
FcConfigSetFonts (FcConfig	*config,
		  FcFontSet	*fonts,
		  FcSetName	set)
{
    if (config->fonts[set])
	FcFontSetDestroy (config->fonts[set]);
    config->fonts[set] = fonts;
}



FcBlanks *
FcConfigGetBlanks (FcConfig	*config)
{
    if (!config)
    {
	config = FcConfigGetCurrent ();
	if (!config)
	    return 0;
    }
    return config->blanks;
}

FcBool
FcConfigAddBlank (FcConfig	*config,
		  FcChar32    	blank)
{
    FcBlanks	*b;
    
    b = config->blanks;
    if (!b)
    {
	b = FcBlanksCreate ();
	if (!b)
	    return FcFalse;
    }
    if (!FcBlanksAdd (b, blank))
	return FcFalse;
    config->blanks = b;
    return FcTrue;
}

int
FcConfigGetRescanInverval (FcConfig *config)
{
    if (!config)
    {
	config = FcConfigGetCurrent ();
	if (!config)
	    return 0;
    }
    return config->rescanInterval;
}

FcBool
FcConfigSetRescanInverval (FcConfig *config, int rescanInterval)
{
    if (!config)
    {
	config = FcConfigGetCurrent ();
	if (!config)
	    return FcFalse;
    }
    config->rescanInterval = rescanInterval;
    return FcTrue;
}

FcBool
FcConfigAddEdit (FcConfig	*config,
		 FcTest		*test,
		 FcEdit		*edit,
		 FcMatchKind	kind)
{
    FcSubst	*subst, **prev;
    FcTest	*t;
    int		num;

    subst = (FcSubst *) malloc (sizeof (FcSubst));
    if (!subst)
	return FcFalse;
    FcMemAlloc (FC_MEM_SUBST, sizeof (FcSubst));
    if (kind == FcMatchPattern)
	prev = &config->substPattern;
    else
	prev = &config->substFont;
    for (; *prev; prev = &(*prev)->next);
    *prev = subst;
    subst->next = 0;
    subst->test = test;
    subst->edit = edit;
    num = 0;
    for (t = test; t; t = t->next)
    {
	if (t->kind == FcMatchDefault)
	    t->kind = kind;
	num++;
    }
    if (config->maxObjects < num)
	config->maxObjects = num;
    if (FcDebug () & FC_DBG_EDIT)
    {
	printf ("Add Subst ");
	FcSubstPrint (subst);
    }
    return FcTrue;
}

typedef struct _FcSubState {
    FcPatternElt   *elt;
    FcValueList    *value;
} FcSubState;

static FcValue
FcConfigPromote (FcValue v, FcValue u)
{
    if (v.type == FcTypeInteger)
    {
	v.type = FcTypeDouble;
	v.u.d = (double) v.u.i;
    }
    else if (v.type == FcTypeVoid && u.type == FcTypeMatrix)
    {
	v.u.m = &FcIdentityMatrix;
	v.type = FcTypeMatrix;
    }
    else if (v.type == FcTypeString && u.type == FcTypeLangSet)
    {
	v.u.l = FcLangSetPromote (v.u.s);
	v.type = FcTypeLangSet;
    }
    return v;
}

FcBool
FcConfigCompareValue (const FcValue	left_o,
		      FcOp		op,
		      const FcValue	right_o)
{
    FcValue	left = left_o;
    FcValue	right = right_o;
    FcBool	ret = FcFalse;
    
    left = FcConfigPromote (left, right);
    right = FcConfigPromote (right, left);
    if (left.type == right.type) 
    {
	switch (left.type) {
	case FcTypeInteger:
	    break;	/* FcConfigPromote prevents this from happening */
	case FcTypeDouble:
	    switch (op) {
	    case FcOpEqual:
	    case FcOpContains:
	    case FcOpListing:
		ret = left.u.d == right.u.d;
		break;
	    case FcOpNotEqual:
	    case FcOpNotContains:
		ret = left.u.d != right.u.d;
		break;
	    case FcOpLess:    
		ret = left.u.d < right.u.d;
		break;
	    case FcOpLessEqual:    
		ret = left.u.d <= right.u.d;
		break;
	    case FcOpMore:    
		ret = left.u.d > right.u.d;
		break;
	    case FcOpMoreEqual:    
		ret = left.u.d >= right.u.d;
		break;
	    default:
		break;
	    }
	    break;
	case FcTypeBool:
	    switch (op) {
	    case FcOpEqual:    
	    case FcOpContains:
	    case FcOpListing:
		ret = left.u.b == right.u.b;
		break;
	    case FcOpNotEqual:
	    case FcOpNotContains:
		ret = left.u.b != right.u.b;
		break;
	    default:
		break;
	    }
	    break;
	case FcTypeString:
	    switch (op) {
	    case FcOpEqual:    
	    case FcOpListing:
		ret = FcStrCmpIgnoreCase (left.u.s, right.u.s) == 0;
		break;
	    case FcOpContains:
		ret = FcStrStrIgnoreCase (left.u.s, right.u.s) != 0;
		break;
	    case FcOpNotEqual:
	    case FcOpNotContains:
		ret = FcStrCmpIgnoreCase (left.u.s, right.u.s) != 0;
		break;
	    default:
		break;
	    }
	    break;
	case FcTypeMatrix:
	    switch (op) {
	    case FcOpEqual:
	    case FcOpContains:
	    case FcOpListing:
		ret = FcMatrixEqual (left.u.m, right.u.m);
		break;
	    case FcOpNotEqual:
	    case FcOpNotContains:
		ret = !FcMatrixEqual (left.u.m, right.u.m);
		break;
	    default:
		break;
	    }
	    break;
	case FcTypeCharSet:
	    switch (op) {
	    case FcOpContains:
	    case FcOpListing:
		/* left contains right if right is a subset of left */
		ret = FcCharSetIsSubset (right.u.c, left.u.c);
		break;
	    case FcOpNotContains:
		/* left contains right if right is a subset of left */
		ret = !FcCharSetIsSubset (right.u.c, left.u.c);
		break;
	    case FcOpEqual:
		ret = FcCharSetEqual (left.u.c, right.u.c);
		break;
	    case FcOpNotEqual:
		ret = !FcCharSetEqual (left.u.c, right.u.c);
		break;
	    default:
		break;
	    }
	    break;
	case FcTypeLangSet:
	    switch (op) {
	    case FcOpContains:
	    case FcOpListing:
		ret = FcLangSetContains (left.u.l, right.u.l);
		break;
	    case FcOpNotContains:
		ret = !FcLangSetContains (left.u.l, right.u.l);
		break;
	    case FcOpEqual:
		ret = FcLangSetEqual (left.u.l, right.u.l);
		break;
	    case FcOpNotEqual:
		ret = !FcLangSetEqual (left.u.l, right.u.l);
		break;
	    default:
		break;
	    }
	    break;
	case FcTypeVoid:
	    switch (op) {
	    case FcOpEqual:
	    case FcOpContains:
	    case FcOpListing:
		ret = FcTrue;
		break;
	    default:
		break;
	    }
	    break;
	case FcTypeFTFace:
	    switch (op) {
	    case FcOpEqual:
	    case FcOpContains:
	    case FcOpListing:
		ret = left.u.f == right.u.f;
		break;
	    case FcOpNotEqual:
	    case FcOpNotContains:
		ret = left.u.f != right.u.f;
		break;
	    default:
		break;
	    }
	    break;
	}
    }
    else
    {
	if (op == FcOpNotEqual || op == FcOpNotContains)
	    ret = FcTrue;
    }
    return ret;
}


#define _FcDoubleFloor(d)	((int) (d))
#define _FcDoubleCeil(d)	((double) (int) (d) == (d) ? (int) (d) : (int) ((d) + 1))
#define FcDoubleFloor(d)	((d) >= 0 ? _FcDoubleFloor(d) : -_FcDoubleCeil(-(d)))
#define FcDoubleCeil(d)		((d) >= 0 ? _FcDoubleCeil(d) : -_FcDoubleFloor(-(d)))
#define FcDoubleRound(d)	FcDoubleFloor ((d) + 0.5)
#define FcDoubleTrunc(d)	((d) >= 0 ? _FcDoubleFloor (d) : -_FcDoubleFloor (-(d)))

static FcValue
FcConfigEvaluate (FcPattern *p, FcExpr *e)
{
    FcValue	v, vl, vr;
    FcResult	r;
    FcMatrix	*m;
    
    switch (e->op) {
    case FcOpInteger:
	v.type = FcTypeInteger;
	v.u.i = e->u.ival;
	break;
    case FcOpDouble:
	v.type = FcTypeDouble;
	v.u.d = e->u.dval;
	break;
    case FcOpString:
	v.type = FcTypeString;
	v.u.s = e->u.sval;
	v = FcValueSave (v);
	break;
    case FcOpMatrix:
	v.type = FcTypeMatrix;
	v.u.m = e->u.mval;
	v = FcValueSave (v);
	break;
    case FcOpCharSet:
	v.type = FcTypeCharSet;
	v.u.c = e->u.cval;
	v = FcValueSave (v);
	break;
    case FcOpBool:
	v.type = FcTypeBool;
	v.u.b = e->u.bval;
	break;
    case FcOpField:
	r = FcPatternGet (p, e->u.field, 0, &v);
	if (r != FcResultMatch)
	    v.type = FcTypeVoid;
	break;
    case FcOpConst:
	if (FcNameConstant (e->u.constant, &v.u.i))
	    v.type = FcTypeInteger;
	else
	    v.type = FcTypeVoid;
	break;
    case FcOpQuest:
	vl = FcConfigEvaluate (p, e->u.tree.left);
	if (vl.type == FcTypeBool)
	{
	    if (vl.u.b)
		v = FcConfigEvaluate (p, e->u.tree.right->u.tree.left);
	    else
		v = FcConfigEvaluate (p, e->u.tree.right->u.tree.right);
	}
	else
	    v.type = FcTypeVoid;
	FcValueDestroy (vl);
	break;
    case FcOpEqual:
    case FcOpNotEqual:
    case FcOpLess:
    case FcOpLessEqual:
    case FcOpMore:
    case FcOpMoreEqual:
    case FcOpContains:
    case FcOpNotContains:
    case FcOpListing:
	vl = FcConfigEvaluate (p, e->u.tree.left);
	vr = FcConfigEvaluate (p, e->u.tree.right);
	v.type = FcTypeBool;
	v.u.b = FcConfigCompareValue (vl, e->op, vr);
	FcValueDestroy (vl);
	FcValueDestroy (vr);
	break;	
    case FcOpOr:
    case FcOpAnd:
    case FcOpPlus:
    case FcOpMinus:
    case FcOpTimes:
    case FcOpDivide:
	vl = FcConfigEvaluate (p, e->u.tree.left);
	vr = FcConfigEvaluate (p, e->u.tree.right);
	vl = FcConfigPromote (vl, vr);
	vr = FcConfigPromote (vr, vl);
	if (vl.type == vr.type)
	{
	    switch (vl.type) {
	    case FcTypeDouble:
		switch (e->op) {
		case FcOpPlus:	   
		    v.type = FcTypeDouble;
		    v.u.d = vl.u.d + vr.u.d; 
		    break;
		case FcOpMinus:
		    v.type = FcTypeDouble;
		    v.u.d = vl.u.d - vr.u.d; 
		    break;
		case FcOpTimes:
		    v.type = FcTypeDouble;
		    v.u.d = vl.u.d * vr.u.d; 
		    break;
		case FcOpDivide:
		    v.type = FcTypeDouble;
		    v.u.d = vl.u.d / vr.u.d; 
		    break;
		default:
		    v.type = FcTypeVoid; 
		    break;
		}
		if (v.type == FcTypeDouble &&
		    v.u.d == (double) (int) v.u.d)
		{
		    v.type = FcTypeInteger;
		    v.u.i = (int) v.u.d;
		}
		break;
	    case FcTypeBool:
		switch (e->op) {
		case FcOpOr:
		    v.type = FcTypeBool;
		    v.u.b = vl.u.b || vr.u.b;
		    break;
		case FcOpAnd:
		    v.type = FcTypeBool;
		    v.u.b = vl.u.b && vr.u.b;
		    break;
		default:
		    v.type = FcTypeVoid; 
		    break;
		}
		break;
	    case FcTypeString:
		switch (e->op) {
		case FcOpPlus:
		    v.type = FcTypeString;
		    v.u.s = FcStrPlus (vl.u.s, vr.u.s);
		    if (!v.u.s)
			v.type = FcTypeVoid;
		    break;
		default:
		    v.type = FcTypeVoid;
		    break;
		}
		break;
	    case FcTypeMatrix:
		switch (e->op) {
		case FcOpTimes:
		    v.type = FcTypeMatrix;
		    m = malloc (sizeof (FcMatrix));
		    if (m)
		    {
			FcMemAlloc (FC_MEM_MATRIX, sizeof (FcMatrix));
			FcMatrixMultiply (m, vl.u.m, vr.u.m);
			v.u.m = m;
		    }
		    else
		    {
			v.type = FcTypeVoid;
		    }
		    break;
		default:
		    v.type = FcTypeVoid;
		    break;
		}
		break;
	    default:
		v.type = FcTypeVoid;
		break;
	    }
	}
	else
	    v.type = FcTypeVoid;
	FcValueDestroy (vl);
	FcValueDestroy (vr);
	break;
    case FcOpNot:
	vl = FcConfigEvaluate (p, e->u.tree.left);
	switch (vl.type) {
	case FcTypeBool:
	    v.type = FcTypeBool;
	    v.u.b = !vl.u.b;
	    break;
	default:
	    v.type = FcTypeVoid;
	    break;
	}
	FcValueDestroy (vl);
	break;
    case FcOpFloor:
	vl = FcConfigEvaluate (p, e->u.tree.left);
	switch (vl.type) {
	case FcTypeInteger:
	    v = vl;
	    break;
	case FcTypeDouble:
	    v.type = FcTypeInteger;
	    v.u.i = FcDoubleFloor (vl.u.d);
	    break;
	default:
	    v.type = FcTypeVoid;
	    break;
	}
	FcValueDestroy (vl);
	break;
    case FcOpCeil:
	vl = FcConfigEvaluate (p, e->u.tree.left);
	switch (vl.type) {
	case FcTypeInteger:
	    v = vl;
	    break;
	case FcTypeDouble:
	    v.type = FcTypeInteger;
	    v.u.i = FcDoubleCeil (vl.u.d);
	    break;
	default:
	    v.type = FcTypeVoid;
	    break;
	}
	FcValueDestroy (vl);
	break;
    case FcOpRound:
	vl = FcConfigEvaluate (p, e->u.tree.left);
	switch (vl.type) {
	case FcTypeInteger:
	    v = vl;
	    break;
	case FcTypeDouble:
	    v.type = FcTypeInteger;
	    v.u.i = FcDoubleRound (vl.u.d);
	    break;
	default:
	    v.type = FcTypeVoid;
	    break;
	}
	FcValueDestroy (vl);
	break;
    case FcOpTrunc:
	vl = FcConfigEvaluate (p, e->u.tree.left);
	switch (vl.type) {
	case FcTypeInteger:
	    v = vl;
	    break;
	case FcTypeDouble:
	    v.type = FcTypeInteger;
	    v.u.i = FcDoubleTrunc (vl.u.d);
	    break;
	default:
	    v.type = FcTypeVoid;
	    break;
	}
	FcValueDestroy (vl);
	break;
    default:
	v.type = FcTypeVoid;
	break;
    }
    return v;
}

static FcValueList *
FcConfigMatchValueList (FcPattern	*p,
			FcTest		*t,
			FcValueList	*values)
{
    FcValueList	    *ret = 0;
    FcExpr	    *e = t->expr;
    FcValue	    value;
    FcValueList	    *v;
    
    while (e)
    {
	/* Compute the value of the match expression */
	if (e->op == FcOpComma)
	{
	    value = FcConfigEvaluate (p, e->u.tree.left);
	    e = e->u.tree.right;
	}
	else
	{
	    value = FcConfigEvaluate (p, e);
	    e = 0;
	}

	for (v = values; v; v = v->next)
	{
	    /* Compare the pattern value to the match expression value */
	    if (FcConfigCompareValue (v->value, t->op, value))
	    {
		if (!ret)
		    ret = v;
	    }
	    else
	    {
		if (t->qual == FcQualAll)
		{
		    ret = 0;
		    break;
		}
	    }
	}
	FcValueDestroy (value);
    }
    return ret;
}

static FcValueList *
FcConfigValues (FcPattern *p, FcExpr *e, FcValueBinding binding)
{
    FcValueList	*l;
    
    if (!e)
	return 0;
    l = (FcValueList *) malloc (sizeof (FcValueList));
    if (!l)
	return 0;
    FcMemAlloc (FC_MEM_VALLIST, sizeof (FcValueList));
    if (e->op == FcOpComma)
    {
	l->value = FcConfigEvaluate (p, e->u.tree.left);
	l->next  = FcConfigValues (p, e->u.tree.right, binding);
    }
    else
    {
	l->value = FcConfigEvaluate (p, e);
	l->next  = 0;
    }
    l->binding = binding;
    while (l && l->value.type == FcTypeVoid)
    {
	FcValueList	*next = l->next;
	
	FcMemFree (FC_MEM_VALLIST, sizeof (FcValueList));
	free (l);
	l = next;
    }
    return l;
}

static FcBool
FcConfigAdd (FcValueList    **head,
	     FcValueList    *position,
	     FcBool	    append,
	     FcValueList    *new)
{
    FcValueList	    **prev, *last, *v;
    FcValueBinding  sameBinding;
    
    if (position)
	sameBinding = position->binding;
    else
	sameBinding = FcValueBindingWeak;
    for (v = new; v; v = v->next)
	if (v->binding == FcValueBindingSame)
	    v->binding = sameBinding;
    if (append)
    {
	if (position)
	    prev = &position->next;
	else
	    for (prev = head; *prev; prev = &(*prev)->next)
		;
    }
    else
    {
	if (position)
	{
	    for (prev = head; *prev; prev = &(*prev)->next)
	    {
		if (*prev == position)
		    break;
	    }
	}
	else
	    prev = head;

	if (FcDebug () & FC_DBG_EDIT)
	{
	    if (!*prev)
		printf ("position not on list\n");
	}
    }

    if (FcDebug () & FC_DBG_EDIT)
    {
	printf ("%s list before ", append ? "Append" : "Prepend");
	FcValueListPrint (*head);
	printf ("\n");
    }
    
    if (new)
    {
	last = new;
	while (last->next)
	    last = last->next;
    
	last->next = *prev;
	*prev = new;
    }
    
    if (FcDebug () & FC_DBG_EDIT)
    {
	printf ("%s list after ", append ? "Append" : "Prepend");
	FcValueListPrint (*head);
	printf ("\n");
    }
    
    return FcTrue;
}

static void
FcConfigDel (FcValueList    **head,
	     FcValueList    *position)
{
    FcValueList    **prev;

    for (prev = head; *prev; prev = &(*prev)->next)
    {
	if (*prev == position)
	{
	    *prev = position->next;
	    position->next = 0;
	    FcValueListDestroy (position);
	    break;
	}
    }
}

static void
FcConfigPatternAdd (FcPattern	*p,
		    const char	*object,
		    FcValueList	*list,
		    FcBool	append)
{
    if (list)
    {
	FcPatternElt    *e = FcPatternInsertElt (p, object);
    
	if (!e)
	    return;
	FcConfigAdd (&e->values, 0, append, list);
    }
}

/*
 * Delete all values associated with a field
 */
static void
FcConfigPatternDel (FcPattern	*p,
		    const char	*object)
{
    FcPatternElt    *e = FcPatternFindElt (p, object);
    if (!e)
	return;
    while (e->values)
	FcConfigDel (&e->values, e->values);
}

static void
FcConfigPatternCanon (FcPattern	    *p,
		      const char    *object)
{
    FcPatternElt    *e = FcPatternFindElt (p, object);
    if (!e)
	return;
    if (!e->values)
	FcPatternDel (p, object);
}

FcBool
FcConfigSubstituteWithPat (FcConfig    *config,
			   FcPattern   *p,
			   FcPattern   *p_pat,
			   FcMatchKind kind)
{
    FcSubst	    *s;
    FcSubState	    *st;
    int		    i;
    FcTest	    *t;
    FcEdit	    *e;
    FcValueList	    *l;
    FcPattern	    *m;

    if (!config)
    {
	config = FcConfigGetCurrent ();
	if (!config)
	    return FcFalse;
    }

    st = (FcSubState *) malloc (config->maxObjects * sizeof (FcSubState));
    if (!st && config->maxObjects)
	return FcFalse;
    FcMemAlloc (FC_MEM_SUBSTATE, config->maxObjects * sizeof (FcSubState));

    if (FcDebug () & FC_DBG_EDIT)
    {
	printf ("FcConfigSubstitute ");
	FcPatternPrint (p);
    }
    if (kind == FcMatchPattern)
	s = config->substPattern;
    else
	s = config->substFont;
    for (; s; s = s->next)
    {
	/*
	 * Check the tests to see if
	 * they all match the pattern
	 */
	for (t = s->test, i = 0; t; t = t->next, i++)
	{
	    if (FcDebug () & FC_DBG_EDIT)
	    {
		printf ("FcConfigSubstitute test ");
		FcTestPrint (t);
	    }
	    st[i].elt = 0;
	    if (kind == FcMatchFont && t->kind == FcMatchPattern)
		m = p_pat;
	    else
		m = p;
	    if (m)
		st[i].elt = FcPatternFindElt (m, t->field);
	    else
		st[i].elt = 0;
	    /*
	     * If there's no such field in the font,
	     * then FcQualAll matches while FcQualAny does not
	     */
	    if (!st[i].elt)
	    {
		if (t->qual == FcQualAll)
		{
		    st[i].value = 0;
		    continue;
		}
		else
		    break;
	    }
	    /*
	     * Check to see if there is a match, mark the location
	     * to apply match-relative edits
	     */
	    st[i].value = FcConfigMatchValueList (m, t, st[i].elt->values);
	    if (!st[i].value)
		break;
	    if (t->qual == FcQualFirst && st[i].value != st[i].elt->values)
		break;
	    if (t->qual == FcQualNotFirst && st[i].value == st[i].elt->values)
		break;
	}
	if (t)
	{
	    if (FcDebug () & FC_DBG_EDIT)
		printf ("No match\n");
	    continue;
	}
	if (FcDebug () & FC_DBG_EDIT)
	{
	    printf ("Substitute ");
	    FcSubstPrint (s);
	}
	for (e = s->edit; e; e = e->next)
	{
	    /*
	     * Evaluate the list of expressions
	     */
	    l = FcConfigValues (p, e->expr, e->binding);
	    /*
	     * Locate any test associated with this field, skipping
	     * tests associated with the pattern when substituting in
	     * the font
	     */
	    for (t = s->test, i = 0; t; t = t->next, i++)
	    {
		if ((t->kind == FcMatchFont || kind == FcMatchPattern) &&
		    !FcStrCmpIgnoreCase ((FcChar8 *) t->field, 
					 (FcChar8 *) e->field))
		{
		    /* 
		     * KLUDGE - the pattern may have been reallocated or
		     * things may have been inserted or deleted above
		     * this element by other edits.  Go back and find
		     * the element again
		     */
		    if (e != s->edit && st[i].elt)
			st[i].elt = FcPatternFindElt (p, t->field);
		    if (!st[i].elt)
			t = 0;
		    break;
		}
	    }
	    switch (e->op) {
	    case FcOpAssign:
		/*
		 * If there was a test, then replace the matched
		 * value with the new list of values
		 */
		if (t)
		{
		    FcValueList	*thisValue = st[i].value;
		    FcValueList	*nextValue = thisValue ? thisValue->next : 0;
		    
		    /*
		     * Append the new list of values after the current value
		     */
		    FcConfigAdd (&st[i].elt->values, thisValue, FcTrue, l);
		    /*
		     * Delete the marked value
		     */
		    FcConfigDel (&st[i].elt->values, thisValue);
		    /*
		     * Adjust any pointers into the value list to ensure
		     * future edits occur at the same place
		     */
		    for (t = s->test, i = 0; t; t = t->next, i++)
		    {
			if (st[i].value == thisValue)
			    st[i].value = nextValue;
		    }
		    break;
		}
		/* fall through ... */
	    case FcOpAssignReplace:
		/*
		 * Delete all of the values and insert
		 * the new set
		 */
		FcConfigPatternDel (p, e->field);
		FcConfigPatternAdd (p, e->field, l, FcTrue);
		/*
		 * Adjust any pointers into the value list as they no
		 * longer point to anything valid
		 */
		if (t)
		{
		    FcPatternElt    *thisElt = st[i].elt;
		    for (t = s->test, i = 0; t; t = t->next, i++)
		    {
			if (st[i].elt == thisElt)
			    st[i].value = 0;
		    }
		}
		break;
	    case FcOpPrepend:
		if (t)
		{
		    FcConfigAdd (&st[i].elt->values, st[i].value, FcFalse, l);
		    break;
		}
		/* fall through ... */
	    case FcOpPrependFirst:
		FcConfigPatternAdd (p, e->field, l, FcFalse);
		break;
	    case FcOpAppend:
		if (t)
		{
		    FcConfigAdd (&st[i].elt->values, st[i].value, FcTrue, l);
		    break;
		}
		/* fall through ... */
	    case FcOpAppendLast:
		FcConfigPatternAdd (p, e->field, l, FcTrue);
		break;
	    default:
		break;
	    }
	}
	/*
	 * Now go through the pattern and eliminate
	 * any properties without data
	 */
	for (e = s->edit; e; e = e->next)
	    FcConfigPatternCanon (p, e->field);

	if (FcDebug () & FC_DBG_EDIT)
	{
	    printf ("FcConfigSubstitute edit");
	    FcPatternPrint (p);
	}
    }
    FcMemFree (FC_MEM_SUBSTATE, config->maxObjects * sizeof (FcSubState));
    free (st);
    if (FcDebug () & FC_DBG_EDIT)
    {
	printf ("FcConfigSubstitute done");
	FcPatternPrint (p);
    }
    return FcTrue;
}

FcBool
FcConfigSubstitute (FcConfig	*config,
		    FcPattern	*p,
		    FcMatchKind	kind)
{
    return FcConfigSubstituteWithPat (config, p, 0, kind);
}

#if defined (_WIN32) && (defined (PIC) || defined (DLL_EXPORT))

static FcChar8 fontconfig_path[1000] = "";

BOOL WINAPI
DllMain (HINSTANCE hinstDLL,
	 DWORD     fdwReason,
	 LPVOID    lpvReserved)
{
  FcChar8 *p;

  switch (fdwReason) {
  case DLL_PROCESS_ATTACH:
      if (!GetModuleFileName ((HMODULE) hinstDLL, fontconfig_path,
			      sizeof (fontconfig_path)))
	  break;

      /* If the fontconfig DLL is in a "bin" or "lib" subfolder,
       * assume it's a Unix-style installation tree, and use
       * "etc/fonts" in there as FONTCONFIG_PATH. Otherwise use the
       * folder where the DLL is as FONTCONFIG_PATH.
       */
      p = strrchr (fontconfig_path, '\\');
      if (p)
      {
	  *p = '\0';
	  p = strrchr (fontconfig_path, '\\');
	  if (p && (FcStrCmpIgnoreCase (p + 1, "bin") == 0 ||
		    FcStrCmpIgnoreCase (p + 1, "lib") == 0))
	      *p = '\0';
	  strcat (fontconfig_path, "\\etc\\fonts");
      }
      else
          fontconfig_path[0] = '\0';
      
      break;
  }

  return TRUE;
}

#undef FONTCONFIG_PATH
#define FONTCONFIG_PATH fontconfig_path

#else /* !(_WIN32 && PIC) */

#endif /* !(_WIN32 && PIC) */

#ifndef FONTCONFIG_FILE
#define FONTCONFIG_FILE	"fonts.conf"
#endif

static FcChar8 *
FcConfigFileExists (const FcChar8 *dir, const FcChar8 *file)
{
    FcChar8    *path;

    if (!dir)
	dir = (FcChar8 *) "";
    path = malloc (strlen ((char *) dir) + 1 + strlen ((char *) file) + 1);
    if (!path)
	return 0;

    strcpy ((char *) path, (const char *) dir);
    /* make sure there's a single separator */
#ifdef _WIN32
    if ((!path[0] || (path[strlen((char *) path)-1] != '/' &&
		      path[strlen((char *) path)-1] != '\\')) &&
	!(file[0] == '/' ||
	  file[0] == '\\' ||
	  (isalpha (file[0]) && file[1] == ':' && (file[2] == '/' || file[2] == '\\'))))
	strcat ((char *) path, "\\");
#else
    if ((!path[0] || path[strlen((char *) path)-1] != '/') && file[0] != '/')
	strcat ((char *) path, "/");
#endif
    strcat ((char *) path, (char *) file);

    FcMemAlloc (FC_MEM_STRING, strlen ((char *) path) + 1);
    if (access ((char *) path, R_OK) == 0)
	return path;
    
    FcStrFree (path);
    return 0;
}

static FcChar8 **
FcConfigGetPath (void)
{
    FcChar8    **path;
    FcChar8    *env, *e, *colon;
    FcChar8    *dir;
    int	    npath;
    int	    i;

    npath = 2;	/* default dir + null */
    env = (FcChar8 *) getenv ("FONTCONFIG_PATH");
    if (env)
    {
	e = env;
	npath++;
	while (*e)
	    if (*e++ == FC_SEARCH_PATH_SEPARATOR)
		npath++;
    }
    path = calloc (npath, sizeof (FcChar8 *));
    if (!path)
	goto bail0;
    i = 0;

    if (env)
    {
	e = env;
	while (*e) 
	{
	    colon = (FcChar8 *) strchr ((char *) e, FC_SEARCH_PATH_SEPARATOR);
	    if (!colon)
		colon = e + strlen ((char *) e);
	    path[i] = malloc (colon - e + 1);
	    if (!path[i])
		goto bail1;
	    strncpy ((char *) path[i], (const char *) e, colon - e);
	    path[i][colon - e] = '\0';
	    if (*colon)
		e = colon + 1;
	    else
		e = colon;
	    i++;
	}
    }
    
    dir = (FcChar8 *) FONTCONFIG_PATH;
    path[i] = malloc (strlen ((char *) dir) + 1);
    if (!path[i])
	goto bail1;
    strcpy ((char *) path[i], (const char *) dir);
    return path;

bail1:
    for (i = 0; path[i]; i++)
	free (path[i]);
    free (path);
bail0:
    return 0;
}

static void
FcConfigFreePath (FcChar8 **path)
{
    FcChar8    **p;

    for (p = path; *p; p++)
	free (*p);
    free (path);
}

static FcBool	_FcConfigHomeEnabled = FcTrue;

FcChar8 *
FcConfigHome (void)
{
    if (_FcConfigHomeEnabled)
    {
        char *home = getenv ("HOME");

#ifdef _WIN32
	if (home == NULL)
	    home = getenv ("USERPROFILE");
#endif

	return home;
    }
    return 0;
}

FcBool
FcConfigEnableHome (FcBool enable)
{
    FcBool  prev = _FcConfigHomeEnabled;
    _FcConfigHomeEnabled = enable;
    return prev;
}

FcChar8 *
FcConfigFilename (const FcChar8 *url)
{
    FcChar8    *file, *dir, **path, **p;
    
    if (!url || !*url)
    {
	url = (FcChar8 *) getenv ("FONTCONFIG_FILE");
	if (!url)
	    url = (FcChar8 *) FONTCONFIG_FILE;
    }
    file = 0;

#ifdef _WIN32
    if (isalpha (*url) &&
	url[1] == ':' &&
	(url[2] == '/' || url[2] == '\\'))
	goto absolute_path;
#endif

    switch (*url) {
    case '~':
	dir = FcConfigHome ();
	if (dir)
	    file = FcConfigFileExists (dir, url + 1);
	else
	    file = 0;
	break;
#ifdef _WIN32
    case '\\':
    absolute_path:
#endif
    case '/':
	file = FcConfigFileExists (0, url);
	break;
    default:
	path = FcConfigGetPath ();
	if (!path)
	    return 0;
	for (p = path; *p; p++)
	{
	    file = FcConfigFileExists (*p, url);
	    if (file)
		break;
	}
	FcConfigFreePath (path);
	break;
    }
    return file;
}

/*
 * Manage the application-specific fonts
 */

FcBool
FcConfigAppFontAddFile (FcConfig    *config,
			const FcChar8  *file)
{
    FcFontSet	*set;
    FcStrSet	*subdirs;
    FcStrList	*sublist;
    FcChar8	*subdir;

    if (!config)
    {
	config = FcConfigGetCurrent ();
	if (!config)
	    return FcFalse;
    }

    subdirs = FcStrSetCreate ();
    if (!subdirs)
	return FcFalse;
    
    set = FcConfigGetFonts (config, FcSetApplication);
    if (!set)
    {
	set = FcFontSetCreate ();
	if (!set)
	{
	    FcStrSetDestroy (subdirs);
	    return FcFalse;
	}
	FcConfigSetFonts (config, set, FcSetApplication);
    }
	
    if (!FcFileScanConfig (set, subdirs, 0, config->blanks, file, FcFalse, config))
    {
	FcStrSetDestroy (subdirs);
	return FcFalse;
    }
    if ((sublist = FcStrListCreate (subdirs)))
    {
	while ((subdir = FcStrListNext (sublist)))
	{
	    FcConfigAppFontAddDir (config, subdir);
	}
	FcStrListDone (sublist);
    }
    return FcTrue;
}

FcBool
FcConfigAppFontAddDir (FcConfig	    *config,
		       const FcChar8   *dir)
{
    FcFontSet	*set;
    FcStrSet	*subdirs;
    FcStrList	*sublist;
    FcChar8	*subdir;
    
    if (!config)
    {
	config = FcConfigGetCurrent ();
	if (!config)
	    return FcFalse;
    }
    subdirs = FcStrSetCreate ();
    if (!subdirs)
	return FcFalse;
    
    set = FcConfigGetFonts (config, FcSetApplication);
    if (!set)
    {
	set = FcFontSetCreate ();
	if (!set)
	{
	    FcStrSetDestroy (subdirs);
	    return FcFalse;
	}
	FcConfigSetFonts (config, set, FcSetApplication);
    }
    
    if (!FcDirScanConfig (set, subdirs, 0, config->blanks, dir, FcFalse, config))
    {
	FcStrSetDestroy (subdirs);
	return FcFalse;
    }
    if ((sublist = FcStrListCreate (subdirs)))
    {
	while ((subdir = FcStrListNext (sublist)))
	{
	    FcConfigAppFontAddDir (config, subdir);
	}
	FcStrListDone (sublist);
    }
    return FcTrue;
}

void
FcConfigAppFontClear (FcConfig	    *config)
{
    if (!config)
    {
	config = FcConfigGetCurrent ();
	if (!config)
	    return;
    }

    FcConfigSetFonts (config, 0, FcSetApplication);
}

/*
 * Manage filename-based font source selectors
 */

FcBool
FcConfigGlobAdd (FcConfig	*config,
		 const FcChar8  *glob,
		 FcBool		accept)
{
    FcStrSet	*set = accept ? config->acceptGlobs : config->rejectGlobs;

    return FcStrSetAdd (set, glob);
}

static FcBool
FcConfigGlobMatch (const FcChar8    *glob,
		   const FcChar8    *string)
{
    FcChar8	c;

    while ((c = *glob++)) 
    {
	switch (c) {
	case '*':
	    /* short circuit common case */
	    if (!*glob)
		return FcTrue;
	    /* short circuit another common case */
	    if (strchr ((char *) glob, '*') == 0)
		string += strlen ((char *) string) - strlen ((char *) glob);
	    while (*string)
	    {
		if (FcConfigGlobMatch (glob, string))
		    return FcTrue;
		string++;
	    }
	    return FcFalse;
	case '?':
	    if (*string++ == '\0')
		return FcFalse;
	    break;
	default:
	    if (*string++ != c)
		return FcFalse;
	    break;
	}
    }
    return *string == '\0';
}

static FcBool
FcConfigGlobsMatch (const FcStrSet	*globs,
		    const FcChar8	*string)
{
    int	i;

    for (i = 0; i < globs->num; i++)
	if (FcConfigGlobMatch (globs->strs[i], string))
	    return FcTrue;
    return FcFalse;
}

FcBool
FcConfigAcceptFilename (FcConfig	*config,
			const FcChar8	*filename)
{
    if (FcConfigGlobsMatch (config->acceptGlobs, filename))
	return FcTrue;
    if (FcConfigGlobsMatch (config->rejectGlobs, filename))
	return FcFalse;
    return FcTrue;
}

/*
 * Manage font-pattern based font source selectors
 */

FcBool
FcConfigPatternsAdd (FcConfig	*config,
		     FcPattern	*pattern,
		     FcBool	accept)
{
    FcFontSet	*set = accept ? config->acceptPatterns : config->rejectPatterns;

    return FcFontSetAdd (set, pattern);
}

static FcBool
FcConfigPatternsMatch (const FcFontSet	*patterns,
		       const FcPattern	*font)
{
    int i;
    
    for (i = 0; i < patterns->nfont; i++)
	if (FcListPatternMatchAny (patterns->fonts[i], font))
	    return FcTrue;
    return FcFalse;
}

FcBool
FcConfigAcceptFont (FcConfig	    *config,
		    const FcPattern *font)
{
    if (FcConfigPatternsMatch (config->acceptPatterns, font))
	return FcTrue;
    if (FcConfigPatternsMatch (config->rejectPatterns, font))
	return FcFalse;
    return FcTrue;
}
