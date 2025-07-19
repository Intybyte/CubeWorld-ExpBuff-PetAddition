// Il blocco ifdef seguente viene in genere usato per creare macro che semplificano
// l'esportazione da una DLL. Tutti i file all'interno della DLL sono compilati con il simbolo ALLOCCRASHFIX_EXPORTS
// definito nella riga di comando. Questo simbolo non deve essere definito in alcun progetto
// che usa questa DLL. In questo modo qualsiasi altro progetto i cui file di origine includono questo file vedranno le funzioni
// le funzioni di ALLOCCRASHFIX_API come se fossero importate da una DLL, mentre questa DLL considera i simboli
// definiti con questa macro come esportati.
#ifdef ALLOCCRASHFIX_EXPORTS
#define ALLOCCRASHFIX_API __declspec(dllexport)
#else
#define ALLOCCRASHFIX_API __declspec(dllimport)
#endif

// Questa classe viene esportata dalla DLL
class ALLOCCRASHFIX_API CAllocCrashFix {
public:
	CAllocCrashFix(void);
	// TODO: aggiungere qui i metodi.
};

extern ALLOCCRASHFIX_API int nAllocCrashFix;

ALLOCCRASHFIX_API int fnAllocCrashFix(void);
