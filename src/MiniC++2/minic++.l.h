typedef union{
	TreeRecord* node;
	char*	ids;
} YYSTYPE;
#define	EXTERN	258
#define	CIN	259
#define	DEFINE	260
#define	BREAK	261
#define	COUT	262
#define	UNDEF	263
#define	FOR	264
#define	TYPEDEF_ID	265
#define	DOUBLE	266
#define	INCLUDE	267
#define	RETURN	268
#define	DO	269
#define	WHILE	270
#define	CONTINUE	271
#define	ID_NAME	272
#define	CLASS_ID	273
#define	INT	274
#define	BOOL_TYPE	275
#define	CHAR	276
#define	STATIC	277
#define	CONST	278
#define	INTEGER_CONST	279
#define	DOUBLE_CONST	280
#define	STRING_CONST	281
#define	CHARACTER_CONST	282
#define	FALSE_CONST	283
#define	TRUE_CONST	284
#define	VOID	285
#define	PUBLIC	286
#define	PROTECTED	287
#define	PRIVATE	288
#define	FRIEND	289
#define	NEW	290
#define	DELETE	291
#define	THIS	292
#define	OPERATOR	293
#define	CLASS	294
#define	INLINE	295
#define	VIRTUAL	296
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


extern YYSTYPE yylval;
