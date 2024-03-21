/*
#ifndef YYSTYPE
#define YYSTYPE char*
#endif
*/
#include "prog_tree.h"
typedef union{
	TreeRecord* node;
	char*	ids;
} YYSTYPE;
#define	CHAR	258
#define	EXTERN	259
#define	BOOL_TYPE	260
#define	CIN	261
#define	CONST	262
#define	DEFINE	263
#define	BREAK	264
#define	COUT	265
#define	UNDEF	266
#define	FOR	267
#define	TYPEDEF_ID	268
#define	VOID	269
#define	DOUBLE	270
#define	INT	271
#define	INCLUDE	272
#define	RETURN	273
#define	DO	274
#define	STATIC	275
#define	WHILE	276
#define	CONTINUE	277
#define	STRING_CONST	278
#define	INTEGER_CONST	279
#define	DOUBLE_CONST	280
#define	CHARACTER_CONST	281
#define	FALSE_CONST	282
#define	TRUE_CONST	283
#define	NEW	284
#define	DELETE	285
#define	THIS	286
#define	OPERATOR	287
#define	CLASS	288
#define	PUBLIC	289
#define	PROTECTED	290
#define	PRIVATE	291
#define	VIRTUAL	292
#define	FRIEND	293
#define	INLINE	294
#define	ID_NAME	295
#define	CLASS_ID	296
#define	INPUT_VAR_SYM	297
#define	OUTPUT_VAR_SYM	298
#define	PLUS_EQ	299
#define	MULT_EQ	300
#define	MINUS_EQ	301
#define	DIV_EQ	302
#define	LE	303
#define	GE	304
#define	EQ	305
#define	NE	306
#define	ANDAND	307
#define	OROR	308
#define	ICR	309
#define	DECR	310
#define	POS_NEG	311
#define	LAST_PREC	312
#define	ARROW	313
#define	SCOPE_DOTS	314
#define	IF	315
#define	ELSE	316



