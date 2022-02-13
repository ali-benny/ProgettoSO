[33mcommit 1688fc3f1154331607b2756ebea0ec53f34c9fcc[m[33m ([m[1;36mHEAD -> [m[1;32mmain[m[33m, [m[1;31morigin/main[m[33m, [m[1;31morigin/HEAD[m[33m)[m
Author: ali-benny <alice17bee@gmail.com>
Date:   Sun Feb 13 12:38:18 2022 +0100

    Proviamo il titolo
    
    provo descrizione, sorry per tutte queste prove XD
    Co-Authored-By: BettoScuderi <34723016+BettoScuderi@users.noreply.github.com>

[33mcommit 70fcae5fd8cd647ac168eba686239c850312b29c[m
Author: ali-benny <alice17bee@gmail.com>
Date:   Sun Feb 13 12:31:05 2022 +0100

    Prova
    
    Provaaa
    Co-authored-by: BettoScuderi <34723016+BettoScuderi@users.noreply.github.com>

[33mcommit 063eba5e01d6d3a75b656b8906ebc75966f2638f[m
Author: ali-benny <alice17bee@gmail.com>
Date:   Sun Feb 13 12:21:10 2022 +0100

    prova pt2
    
    descrizione

[33mcommit d87ef9eff2917df0fe84108f96800d5e5fa971d5[m
Author: ali-benny <alice17bee@gmail.com>
Date:   Sun Feb 13 12:18:32 2022 +0100

    prova
    
    descrizione

[33mcommit dca7ec4735954284c2edc041b3502b7224e9839d[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Sat Feb 12 14:33:56 2022 +0100

    fixed const.h not found + fixed warning container_of
    
    Fixed const.h:
    - :pushpin:  #define UMPS3 -> in utils.h
    - aggiunti ifdef UMPS3 a pandos_const.h
    :heavy_plus_sign:  trovato Seg Fault in stampaLista :/
    - rimosso rimasugli di getHead sparsi per i .h
    
    Fixed warning container_of:
    - dava errore il tipo size_tt, fixato con: typedef intptr_t size_tt;

[33mcommit 0b34a14e5d4acaf4c7e03008f2c8988a36cccf9a[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Fri Feb 11 13:10:12 2022 +0100

    fixed main
    
    abbiamo "cancellato" main.c risuddividendo i main nei rispettivi file ASL.c e PCB.c
    - inserito stampaListe in ogni file.c perchè è stupido e non lo trovava se era solo nel utils.c (che si può benissimo cancellare)
    
    - aggiornato makefile di Libera togliendo main.c
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>

[33mcommit 182805a4fc42876068ecc09e53cb7cf13af12fe1[m
Author: ali-benny <alice17bee@gmail.com>
Date:   Fri Feb 11 12:10:05 2022 +0100

    p1test test2
    
    non riesce ad includere const.h

[33mcommit 3887fab93c2632bf46cf01a42fb5e50cc686a9ae[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Fri Feb 11 11:43:40 2022 +0100

    Prova test umps
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>
    Co-Authored-By: Budinooo <62710424+Budinooo@users.noreply.github.com>
    Co-Authored-By: BettoScuderi <34723016+BettoScuderi@users.noreply.github.com>

[33mcommit dc551471c41f44328877d3004fb20f18a0650799[m
Author: LiberaLongo <liberalongo@gmail.com>
Date:   Tue Feb 8 19:49:33 2022 +0100

    risolti errori di compilazione ma dice segmentation fault

[33mcommit 03374ec53f15d580f44b7fdf1ad1a45b5ff16578[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Tue Feb 8 19:34:03 2022 +0100

    getHead con #define DEBUG
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>

[33mcommit 5855219c4689dfa2f3c65affc89f30a6e4460ba3[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Tue Feb 8 18:50:32 2022 +0100

    prova declaration

[33mcommit a46190a326ca9b9eb00831a09558aa8a7d042816[m
Merge: e9b0482 db6158b
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Tue Feb 8 13:57:15 2022 +0100

    Merge branch 'main' of https://github.com/ali-benny/ProgettoSO

[33mcommit e9b0482df47c23ebdde3089b1d7158b4446b010d[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Fri Feb 4 13:25:49 2022 +0100

    minifix

[33mcommit db6158b629211b91743c09f470d508cb607c04a2[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Fri Feb 4 13:25:49 2022 +0100

    minifix

[33mcommit 4573a2761657b17e21da34dee0408a95e8ecdaae[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Fri Feb 4 13:00:09 2022 +0100

    test per include error

[33mcommit 84bcd12f07fcf5faef173137676239aef349b4e9[m
Author: LiberaLongo <liberalongo@gmail.com>
Date:   Fri Feb 4 12:43:59 2022 +0100

    inclusione troppe volte della stessa cosa non si capisce pk

[33mcommit 30444b8e4b9dc346d6189f198a5e7adbd1923925[m
Author: LiberaLongo <liberalongo@gmail.com>
Date:   Fri Feb 4 12:19:19 2022 +0100

    implementata insertSem() pcbFree_h sembra ridefinita quindi non va il make

[33mcommit c65e31e5cb41768b3f9e218a120b1cf41ffb1e00[m
Author: LiberaLongo <liberalongo@gmail.com>
Date:   Fri Feb 4 11:07:05 2022 +0100

    inizio ASL.h e ASL.c

[33mcommit 601a5cd03745178deff163a0c0641a4208970831[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Wed Feb 2 15:51:25 2022 +0100

    committo speriamo che domani si sia fixato windows
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>
    Co-Authored-By: Budinooo <62710424+Budinooo@users.noreply.github.com>

[33mcommit 7046f4ee9df6afd09f6d2a290c068953458f0bb9[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Wed Feb 2 12:57:53 2022 +0100

    fine Alberi -> alloc esplosa :/
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>
    Co-Authored-By: Budinooo <62710424+Budinooo@users.noreply.github.com>

[33mcommit a545fe6b1882f8df19f97652a95c9d7a3e07d9d6[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Wed Feb 2 12:12:10 2022 +0100

    pre outChild
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>
    Co-Authored-By: Budinooo <62710424+Budinooo@users.noreply.github.com>
    Co-Authored-By: BettoScuderi <34723016+BettoScuderi@users.noreply.github.com>

[33mcommit 42dfcff750ee405e31eac3fddaff94d445865ed2[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Tue Feb 1 13:09:07 2022 +0100

    insertChild pt2
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>
    Co-Authored-By: Budinooo <62710424+Budinooo@users.noreply.github.com>
    Co-Authored-By: BettoScuderi <34723016+BettoScuderi@users.noreply.github.com>

[33mcommit 3e3d1c4eccd7896dfd88d4778e14b49ce3e6a4a8[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Tue Feb 1 11:49:07 2022 +0100

    p_child.next==NULL non va in insertChild
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>
    Co-Authored-By: Budinooo <62710424+Budinooo@users.noreply.github.com>
    Co-Authored-By: BettoScuderi <34723016+BettoScuderi@users.noreply.github.com>

[33mcommit 55c3482d9a6a1bbaa2b9bfe6175cfb87b3607667[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Fri Jan 28 17:53:53 2022 +0100

    inizio funzioni alberi pcb
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>
    Co-Authored-By: Budinooo <62710424+Budinooo@users.noreply.github.com>

[33mcommit 6926f6d6563d465a7869e7f3c193e8c235aa475c[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Fri Jan 28 13:06:46 2022 +0100

    CREDIAMO che stia funzionando
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>
    Co-Authored-By: BettoScuderi <34723016+BettoScuderi@users.noreply.github.com>
    Co-Authored-By: Budinooo <62710424+Budinooo@users.noreply.github.com>

[33mcommit 22755b3d4cea339ccfdf4df1a1a11cda8313e2cb[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Fri Jan 28 12:29:40 2022 +0100

    outProcQ test
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>
    Co-Authored-By: BettoScuderi <34723016+BettoScuderi@users.noreply.github.com>
    Co-Authored-By: Budinooo <62710424+Budinooo@users.noreply.github.com>

[33mcommit 8eb932a14022859a5c72587bf582d0713dcbf757[m
Author: ali-benny <alice17bee@gmail.com>
Date:   Thu Jan 27 13:22:19 2022 +0100

    DOVREBBE funzionare FreePCB grazie a dio e a gerald

[33mcommit 61845902acbc64376183a3b2ae08f5fd0d8240db[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Thu Jan 27 13:01:48 2022 +0100

    FreePCB test
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>
    Co-Authored-By: Budinooo <62710424+Budinooo@users.noreply.github.com>

[33mcommit 1f3918405438e355c7c0110067e1559a5461df32[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Thu Jan 27 12:10:05 2022 +0100

    Alloc OK FreePCB OK
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>
    Co-Authored-By: Budinooo <62710424+Budinooo@users.noreply.github.com>
    Co-Authored-By: BettoScuderi <34723016+BettoScuderi@users.noreply.github.com>

[33mcommit 7472edbf779733a442f4d20c6dde03235f4f3fdd[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Thu Jan 27 11:44:05 2022 +0100

    Alloc fixed? capiamo
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>
    Co-Authored-By: Budinooo <62710424+Budinooo@users.noreply.github.com>
    Co-Authored-By: BettoScuderi <34723016+BettoScuderi@users.noreply.github.com>

[33mcommit 6510bcf85466c269e9d4a5ab0eab12042a82c93e[m
Author: ali-benny <alice17bee@gmail.com>
Date:   Wed Jan 26 13:31:27 2022 +0100

    dobbiamo finire di fixare conteiner of in alloc

[33mcommit c1cae566aed3c90e123bfb49b0079d67ecd75bfb[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Wed Jan 26 12:45:21 2022 +0100

    alloc not fixed -> controllo init PCBS
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>
    Co-Authored-By: Budinooo <62710424+Budinooo@users.noreply.github.com>
    Co-Authored-By: BettoScuderi <34723016+BettoScuderi@users.noreply.github.com>

[33mcommit fa8f697e346b1bdf5465cffe7fa926f970d11994[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Wed Jan 26 12:35:38 2022 +0100

    alloc not fixed -> controllo init PCBS
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>
    Co-Authored-By: Budinooo <62710424+Budinooo@users.noreply.github.com>
    Co-Authored-By: BettoScuderi <34723016+BettoScuderi@users.noreply.github.com>

[33mcommit 003056c17a7ccee828ac1ae1d1d064d1af43fce1[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Wed Jan 26 12:08:05 2022 +0100

    alloc fixed
    
    una cazzo di & nel mail
    
    ~cit Gerald
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>
    Co-Authored-By: Budinooo <62710424+Budinooo@users.noreply.github.com>
    Co-Authored-By: BettoScuderi <34723016+BettoScuderi@users.noreply.github.com>

[33mcommit 809f432aac8a5594bd6fd9e3f9359d99a920e139[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Wed Jan 26 11:54:20 2022 +0100

    test pcbFree e pcbFree_h
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>
    Co-Authored-By: Budinooo <62710424+Budinooo@users.noreply.github.com>
    Co-Authored-By: BettoScuderi <34723016+BettoScuderi@users.noreply.github.com>

[33mcommit b1c12b1813b4ff48cdac3d255b9e5b4dac38df7d[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Wed Jan 26 11:29:10 2022 +0100

    test conteiner_of
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>
    Co-Authored-By: Budinooo <62710424+Budinooo@users.noreply.github.com>

[33mcommit b17c6eb4e58fbd4b5c19ee64ffe39f76806e8777[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Wed Jan 26 11:13:28 2022 +0100

    prova rikaya.h
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>

[33mcommit 07225799526414d0768022b412d73a104102a7d9[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Mon Jan 24 12:27:33 2022 +0100

    alloc Fixed
    
    troppo forti ceh, ma
    
    CAZZO NON DICHIARARE UNA VARIABILE LOCAL IN UN IF ceh
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>

[33mcommit 9f0261581bc2ada421bfdd269071f81d65d673fb[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Mon Jan 24 12:18:46 2022 +0100

    pcb.c aggiunto controlli su p e head null
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>

[33mcommit b6be1fc44c88d0ff44265718ccae3ff72d18e1e0[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Mon Jan 24 12:06:42 2022 +0100

    test pt1 per allocPCB
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>

[33mcommit 22e5b58cea76747df17c6fc77deef97a8a3bd202[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Mon Jan 24 11:38:14 2022 +0100

    makefile pt5 XD
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>

[33mcommit 885a6f97291ea1e1795690243eb1efbfa9556e45[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Mon Jan 24 11:26:53 2022 +0100

    Prove:
    
    - rinominato makefile di ieri in makefile_umps
    - creato makefile da usare senza umps
    Andiamo a testare tutto su linux e vediamo :+1:
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>
    Co-Authored-By: Budinooo <62710424+Budinooo@users.noreply.github.com>

[33mcommit e054c0277532aeec47e55fb27e42b2db49484ead[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Sun Jan 23 13:24:04 2022 +0100

    pt4
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>

[33mcommit 5f0c58133f2ddf6a73bb9b9ee805101b081da596[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Sun Jan 23 13:22:40 2022 +0100

    makefile pt3 tab
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>

[33mcommit c84f076b65eea9ecb3720d0d0f29421ef69e6917[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Sun Jan 23 13:19:55 2022 +0100

    update makefile pt2
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>

[33mcommit 76cf216d12703c155df9d380df17995f5fe5b74f[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Sun Jan 23 13:15:37 2022 +0100

    upadate makefile with umps3
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>

[33mcommit 00a46f44abb89cb725a16d840287d0c02025dde8[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Sun Jan 23 12:29:00 2022 +0100

    container_of test
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>
    Co-Authored-By: BettoScuderi <34723016+BettoScuderi@users.noreply.github.com>

[33mcommit 52084c3bf19e30fb39e2d53bb86b040eb4e0d552[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Sun Jan 23 12:19:15 2022 +0100

    state_t
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>
    Co-Authored-By: BettoScuderi <34723016+BettoScuderi@users.noreply.github.com>

[33mcommit 62baae2c590e86f413493417154e2659e41da4a9[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Sun Jan 23 11:28:35 2022 +0100

    define UMPS3 per compilazione
    
    in pandos_const.h abbiamo aggiunto una costante UMPS3 da definire o meno in base a dove viene eseguito/compilato il progetto
    
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>

[33mcommit 2609529dc0a83f6978819185ff3cb246e3744ce4[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Sun Jan 23 11:10:11 2022 +0100

    Cancellati .exe

[33mcommit 8a064a14951bbaa70c36c1f52d0aeda990312a89[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Sun Jan 23 10:53:26 2022 +0100

    Rename file

[33mcommit 59eca5070a9f963459d0c894a2092ef1de617099[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Sun Jan 23 10:50:48 2022 +0100

    Creazione file per test

[33mcommit 3a72ce715c84b003adf311ba12ccce76b6d483c8[m
Author: LiberaLongo <liberalongo@gmail.com>
Date:   Sat Jan 22 15:09:14 2022 +0100

    makefile works!

[33mcommit 95c72535800488fe062f1d7b6f338276723def4e[m
Author: LiberaLongo <liberalongo@gmail.com>
Date:   Sat Jan 22 13:56:57 2022 +0100

    makefile

[33mcommit 8cd4a7fc14867c0cdebb8501e209f33f6bede982[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Sat Jan 22 12:44:39 2022 +0100

    Fixato tutte le funzioni XD
    
    Co-Authored-By: BettoScuderi <34723016+BettoScuderi@users.noreply.github.com>
    Co-Authored-By: Budinooo <62710424+Budinooo@users.noreply.github.com>
    Co-Authored-By: Libera Longo <34750957+LiberaLongo@users.noreply.github.com>

[33mcommit bd976071927f5315ebc97da86fee7e563c741ad7[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Fri Jan 21 17:35:43 2022 +0100

    Sviluppo funzioni 1,2,3
    
    Funzione 3 da warning :(
    
    NB: pandos_h pcb_t commentati cose per testare c non solo umps3

[33mcommit f370c4cfa337eeb94380c626e24352ad2305e33a[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Fri Jan 21 15:47:15 2022 +0100

    Aggiunto funzioni base con relativi commenti

[33mcommit 4e776203f65db8188af8bd49bb0ce487413fdcdd[m
Author: Gerald <gerald.manzano@studio.unibo.it>
Date:   Tue Jan 18 12:11:56 2022 +0100

    prova push

[33mcommit be0750b2a2a58d457dc27e76f5ecd12410ba9d42[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Tue Jan 18 11:39:51 2022 +0100

    creazione PCB.c

[33mcommit 461ba8375ab5edecb8b5588f5770b419df086d60[m
Author: ali-benny <alice17bee@gmail.com>
Date:   Tue Jan 18 11:33:26 2022 +0100

    Aggiunto file README

[33mcommit d364c71c5a5704b5081db1cc0050e98e3e6da632[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Tue Jan 18 10:58:59 2022 +0100

    slide fase1

[33mcommit 652eee0f1ae5311c11eaf2cd328009cfde037726[m
Author: alice benatti <74602443+ali-benny@users.noreply.github.com>
Date:   Tue Jan 18 10:57:58 2022 +0100

    File base fase 1
