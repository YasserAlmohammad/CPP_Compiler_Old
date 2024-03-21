class TreeRecord;
typedef union{
	TreeRecord* node;
	char*	ids;
} YYSTYPE;

#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

#define	EXTERN	258
#define	NULL_TYPE	259
#define	CIN	260
#define	DEFINE	261
#define	BREAK	262
#define	COUT	263
#define	UNDEF	264
#define	FOR	265
#define	TYPEDEF_ID	266
#define	DOUBLE	267
#define	INCLUDE	268
#define	RETURN	269
#define	DO	270
#define	WHILE	271
#define	CONTINUE	272
#define	ID_NAME	273
#define	CLASS_ID	274
#define	INT	275
#define	BOOL_TYPE	276
#define	CHAR	277
#define	STATIC	278
#define	CONST	279
#define	INTEGER_CONST	280
#define	DOUBLE_CONST	281
#define	STRING_CONST	282
#define	CHARACTER_CONST	283
#define	FALSE_CONST	284
#define	TRUE_CONST	285
#define	VOID	286
#define	PUBLIC	287
#define	PROTECTED	288
#define	PRIVATE	289
#define	FRIEND	290
#define	NEW	291
#define	DELETE	292
#define	THIS	293
#define	OPERATOR	294
#define	CLASS	295
#define	INLINE	296
#define	VIRTUAL	297
#define	INPUT_VAR_SYM	298
#define	OUTPUT_VAR_SYM	299
#define	PLUS_EQ	300
#define	MULT_EQ	301
#define	MINUS_EQ	302
#define	DIV_EQ	303
#define	LE	304
#define	GE	305
#define	EQ	306
#define	NE	307
#define	OROR	308
#define	ANDAND	309
#define	POS_NEG	310
#define	ICR	311
#define	DECR	312
#define	LAST_PREC	313
#define	ARROW	314
#define	SCOPE_DOTS	315
#define	IF	316
#define	ELSE	317


extern YYSTYPE yylval;
