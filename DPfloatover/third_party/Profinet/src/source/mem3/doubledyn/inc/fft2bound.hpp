/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
/*  This program is protected by German copyright law and international      */
/*  treaties. The use of this software including but not limited to its      */
/*  Source Code is subject to restrictions as agreed in the license          */
/*  agreement between you and Siemens.                                       */
/*  Copying or distribution is not allowed unless expressly permitted        */
/*  according to your license agreement with Siemens.                        */
/*****************************************************************************/
/*                                                                           */
/*  P r o j e c t         &P: PROFINET IO Runtime Software              :P&  */
/*                                                                           */
/*  P a c k a g e         &W: PROFINET IO Runtime Software              :W&  */
/*                                                                           */
/*  C o m p o n e n t     &C: pcIOX (PNIO Controler, Device and more)   :C&  */
/*                                                                           */
/*  F i l e               &F: fft2bound.hpp                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  MEM3 memory management                                                   */
/*                                                                           */
/*****************************************************************************/

#ifndef FFT2BOUND_HPP
#define FFT2BOUND_HPP

/* Kodierung von cFft1Bnd:
     Zahlen haben folgende Darstellung in der 1.-ten Kodierstufe:
         1.....10b.....b
          n-mal   n-mal
       Die Binaerziffern 'b.....b' sind als n-stellige natuerl. Zahl im 2-Kompl.
       kodiert; sie bilden den Mantissen-Wert M. Der Wert der gesamten Zahl
       errechnet sich als M + 2^n - 1 ('^' == hoch). Ist n == 0, so ist der
       Gesamtwert 0.

       Beispiele:
         11010          ==  2 + 2^2 - 1 ==  5
         111101011      == 11 + 2^4 - 1 == 24
         0              ==  0 + 2^0 - 1 ==  0

     Zu jedem Granulat im Memorybereich gibt es ein BYTE im Verwaltungsbereich. Die
     Granulate im Memory- und BYTEs im Verwaltungsbereich sind als ARRAY angeordnet.
     Granulat und zugehoeriges Verwaltungsbyte haben den gleichen Index.

     Die Puffer- und Freibereiche im Memorybereich werden durch Cluster- und
     Blocklisten verwaltet. In der Clusterliste wechseln sich Frei- und Pufferelemente
     ab, d.h. auf ein Freielement folgt zwingend ein Pufferelement und umgekehrt.

     Ein Freielement aus der Clusterliste ist nicht weiter unterteilt. Ein Puffer-
     element aus der Clusterliste ist seinerseits als Blockliste aufgebaut. Ein
     Element der Blockliste steht fuer genau einen Puffer, der beim Anwender steht.

     Prinzipielle Kodierung der Listen:

          l+1 Elemente       m+1 Elemente    n+1 Elemente       0+1 == 1 Element
          __             __  __          __  __             __  __
         | l|...........|-l|| m|........|-m|| n|...........|-n|| 0|

       Elemente bedeutet Granulate im Memorybereich u. BYTEs im Verwaltungsbereich.
       Die Listen, selbst, sind im Verwaltungsbereich. Ein Listeneintrag ist mindestens
       1-elementig (s.o.).

     Tatsaechliche Kodierung der Listen:
       Da die Zahleninformationen zur Listenkodierung so nicht in einem BYTE unter-
       gebracht werden koennen, werden sie mit steigender Elementgroesse auf immer
       mehr BYTES verteilt.

       Ein Kodierelement (BYTE) hat folgende vertikale Aufteilung:
         3 BIT ... Clusterlaengenkodierung
         3 BIT ... Blocklaengenkodierung
         1 BIT ... Schritt-Vorzeichen
         1 BIT ... Blockstartinfo

       Ist das Schritt-Vorzeichen '0' (positiv), so reicht das Element von hier zu
       einem hoeheren Index. Weiterfuehrende Code-Elemente werden durch Index-
       Increment erreicht und eine kodierte natuerl. Zahl ist so aufzufassen wie
       kodiert.

       Ist das Schritt-Vorzeichen '1' (negativ), so reicht das Element von hier zu
       einem niedrigeren Index. Weiterfuehrende Code-Elemente werden durch Index-
       Decrement erreicht und eine kodierte natuerl. Zahl ist mit '-1' zu multi-
       plizieren.

       Da ein 1-stelliges Listenelement durch '0' kodiert ist, ist es prinzipiell nicht
       von einem unkodierten Zwischenbyte unterscheidbar. Aus diesem Grund ist eine
       Start- und Endeinfo (irgendwie geartet) fuer jedes Listenelement erforderlich.

       Die Clusterstart- und Endeinfo ergibt sich implizit daraus, dass ein nicht
       leerer immer mit einem Cluster beginnt. Von diesem Cluster laesst sich auf den
       naechsten schliessen usw..

       Von einem Cluster ist jedoch nicht implizit klar, ob es sich um einen Frei- oder
       Puffercluster handelt. Jeder Puffercluster besteht jedoch vollstaendig aus einer
       Blockliste. Ferner soll fuer einen Block (Anwenderpuffer) bei der Rueckgabe
       ueberprueft werden koennen, ob es sich denn tatsaechlich um einen (belegten)
       Blockanfang handelt.
       Aus diesen Gruenden sind die Anfangs- und Endebytes eines Blockelementes durch
       Blockstartinfo == '1' markiert. Alle anderen Blockstart-Bits sind '0'.

     Zahlenkodierung in Listen:
       Die Zahlen liegen im Code der 1.-ten Kodierstufe ueber 1 oder mehrere BYTEs
       verteilt in den Listenelementen vor, entsprechend der vertikalen Aufteilung
       je nach Liste entweder in der Clusterlaengenkodierung oder in der Blocklaengen-
       kodierung.
       Dabei liegen im Kopf/Ende-BYTE die hoechstwertigen Stellen und in auf/ab-
       steigenden BYTEs die entsprechend niederwertigen Stellen.

       Beispiel fuer 2 Blockelemente in einem Clusterelement:
           | Block1: 6 Bytes |  Block2: 19 Bytes |
           |        Cluster: 25 Bytes            |

         Kodierung:
           Die Elemente sind immer n+1-stellig. Daraus ergeben sich folgende
           Zahlen und Codes:
             11010          ==  2 + 2^2 - 1 ==  5 ==  6 - 1
             111100011      ==  3 + 2^4 - 1 == 18 == 19 - 1
             111101011      == 11 + 2^4 - 1 == 24 == 25 - 1

           -------------------------------------------
           Start-B 7 |1|0|0|0|1|1|0|0|0|     |0|0|0|1|
           -----------------------------     ---------
                   6 |1|1|x|1|1|1|1|0|x|     |x|0|1|1|
           Block   5 |1|0|x|0|1|1|0|1|x| ... |x|1|0|1|
                   4 |0|x|x|x|0|1|0|1|x|     |x|1|0|1|
           -----------------------------     ---------
           Start-C 3 |1|0|0|0|0|0|0|0|0|     |0|0|0|1|
           -----------------------------     ---------
                   2 |1|1|0|x|x|x|x|x|x|     |x|0|1|1|
           Cluster 1 |1|0|1|x|x|x|x|x|x|     |x|1|0|1|
                   0 |1|1|1|x|x|x|x|x|x|     |x|1|1|1|
           -------------------------------------------
                     | | | | | | | | | |     |2|2|2|2|
           Index +   |0|1|2|3|4|5|6|7|8| ... |1|2|3|4|
                     |         |                     |

           Die mit 'x' gefuellten Felder werden bei Dekodierung nicht ausgewertet;
           sie sind aber mit '0' gefuellt.

           Die hier gezeigte Anordnung der Bitfelder im BYTE entspricht der Intel-
           Nummerierung: BIT-i ist die Stelle 2^i. Die Art der Nummerierung ist
           compilerabhaengig. So nummeriert GNU wie IBM: BIT-i ist die Stelle 2^(n-i),
           wobei n die Breite des Elements ist.
           Das Format kann also nicht zwischen verschiedenen Platformen transferiert
           werden.

       Ist ein Listenelement 1-stellig, so ist grundsaetzlich das Vorzeichen belanglos,
       da die Zahl (Ende-Offset) dann 0 ist.
       Normalerweise wird es als '0' (positiv) kodiert. Eine Ausnahme ist folgende
       Situation:
         Ein 1-elementiges Blockelement faellt zusammen mit dem Ende-BYTE eines
         mehr-elementigen Clusterelements.
         In diesem Fall ist das Schritt-Vorzeichen als '1' (negativ) kodiert.

       Da Cluster- und Blockelemente sich das Schrittvorzeichen gegebenenfalls teilen,
       ist dieser Sonderfall nicht vermeidbar.

   Allgemeiner Aufbau:
   cFft1Bnd ist vektoriell beschrieben. Aus diesem Grund kann ein Objekt normal oder
   revers erzeugt werden.
   Normale Objekte greifen auf die Verwaltungsinfo ab Index 0 bis Index n-1 zu;
   Speicher wird von unten nach oben vergeben.
   Reverse Objekte greifen auf die Verwaltungsinfo ab Index n-1 bis Index 0 zu;
   Speicher wird von oben nach unten vergeben. Puffer werden von inversen Objekten
   durch negative Groessen (-Laenge) als Parameter angefordert. Der Rueck- bzw. Ueber-
   gabeindex ist nicht der 0.-te des Puffers sondern der letzte (i-1).

   Saemtliche Range/Outrange-Bedingungen duerfen deshalb nur durch Betragsbildung
   ueberprueft werden. 'mDirSign' wird waehrend der Konstruktion angelegt und ist '1'
   bzw. '-1' bei normalen bzw. reversen Objekten.
   Abfragen, s.o., erfolgen in der Regel durch heranmultiplizieren von 'mDirSign'.

   'mGranBnd' ist der 1.-te aktuell nicht mehr mit Pufferbereichen verwendete Index.
   'mGranOut' ist der 1.-te aktuell nicht mehr fuer Puffer verfuegbare Index.
   Fuer normale Objekte heisst das: ab hier aufwaerts, fuer reverse: ab hier abwaerts.
*/
class  cFft1Bnd
  {
  public:
    enum eEncodeLen
      {
        eShClustLenBits = 3,
        eShCluStartBits = 1,
        eShBlockLenBits = 3,
        eShBufStartBits = 1
      };

    enum eEncodePos
      {
        eShClustLen = 0,
        eShCluStart = eShClustLen + eShClustLenBits,
        eShBlockLen = eShCluStart + eShCluStartBits,
        eShBufStart = eShBlockLen + eShBlockLenBits
      };

    enum eEncodeStart
      {
        eShClustLenStart = eShClustLen + eShClustLenBits - 1,
        eShBlockLenStart = eShBlockLen + eShBlockLenBits - 1
      };

    struct cEntry
      {
        int  Pos;
        int  End;

             cEntry (int  P, int  E) : Pos (P), End (E) {}
             cEntry () {}
      };

  private:
            ORD08      * const    mGranArray;
            int          const    mDirSign;
            int          const    mGranStart;
            int volatile const &  mGranOut;
            int                   mGranBnd;
            int                   mGranEnd;
            int                   mGranUsed;

    static  int                   Decode            (ORD08  const *  CodeArr, int  Step,   int  MStart, int  MLen);
    static  void                  Encode            (ORD08        *  CodeArr, int  Num,    int  MStart, int  MLen);
    static  int                   DecodeClust       (ORD08  const *  CodeArr, int  Step);
    static  int                   DecodeBlock       (ORD08  const *  CodeArr, int  Step);
    static  void                  EncodeClust       (ORD08        *  CodeArr, int  Num);
    static  void                  EncodeBlock       (ORD08        *  CodeArr, int  Num);
    static  void                  Reset             (ORD08        *  CodeArr, int  Step,   int  MStart, int  MLen);
    static  int                   GetCode           (ORD08           Code,    int  MStart, int  MLen);
    static  void                  PutCode           (ORD08        &  Code,    int  MStart, int  MLen,   int  Num);
    static  bool                  CluStart          (ORD08           Code);
    static  bool                  BufStart          (ORD08           Code);

            void                  ClearBlock        (cEntry const &  Entry);
            void                  ClearCluster      (cEntry const &  Entry);
            void                  SetupBlock        (cEntry const &  Entry);
            void                  SetupCluster      (cEntry const &  Entry);
            void                  InitEmptyCluster  (int  Num);

            void                  Put1st_Inner      (int  Pos, int  End);

  public:
                                  cFft1Bnd          (ORD08 *  GranArray, int  Num, int      const &  GranOut,  bool  GenCluster = true);
                                  cFft1Bnd          (ORD08 *  GranArray, int  Num, cFft1Bnd const &  AntiPool, bool  GenCluster = true);
    virtual                      ~cFft1Bnd          () {}

            int                   Get1stGranAndFix  (int      Num);
            int                   GetEndGranAndFix  (int      Num);
            void                  Put1stGranAndFix  (int      Pos);
            void                  PutEndGranAndFix  (int      End);
            int                   GranStart         () const { return mGranStart; }
            int                   GranBnd           () const { return mGranBnd; }
            int                   GranOut           () const { return mGranOut; }
            int                   GranUsed          () const { return mGranUsed; }
            int                   DirSign           () const { return mDirSign; }
            int                   GetBufferBase     (int      Pos)  const;
            int                   GetBufferEnd      (int      Pos)  const;
            int                   GetBufferLenBase  (int      Pos)  const;
            int                   GetBufferLenEnd   (int      End)  const;
            int                   GetFreeLenBase    (int      Pos)  const;
            int                   GetFreeLenEnd     (int      End)  const;
            int                   GetUsedLenBase    (int      Pos)  const;
            int                   GetUsedLenEnd     (int      End)  const;
            int                   GetNextBufStart   (int      Pos)  const;
            int                   GetNextBufEnd     (int      End)  const;
  };

class  cFft2Bnd
  {
            ORDHW        const    mGranNum;
            ORD08      * const    mGranArray;
            cFft1Bnd              mPoolNormal;
            cFft1Bnd              mPoolReverse;

  public:
                                  cFft2Bnd          (ORD08 *  GranArray, ORDHW  GranNum);
    virtual                      ~cFft2Bnd          () {}

            ORDHW                 GetGranNormal     (ORDHW    Num);
            ORDHW                 GetGranReverse    (ORDHW    Num);
            int                   GetFreeLenNormal  (int      Pos)  const;
            int                   GetFreeLenReverse (int      End)  const;
            int                   GetUsedLenNormal  (int      Pos)  const;
            int                   GetUsedLenReverse (int      End)  const;
            int                   GranOutNormal     () const { return mPoolNormal. GranOut (); }
            int                   GranOutReverse    () const { return mPoolReverse.GranOut (); }
            int                   GranBndNormal     () const { return mPoolNormal. GranBnd (); }
            int                   GranBndReverse    () const { return mPoolReverse.GranBnd (); }
            int                   GetBufStrtNormal  (int      Pos)  const { return mPoolNormal.  GetNextBufStart (Pos); }
            int                   GetBufStrtReverse (int      End)  const { return mPoolReverse. GetNextBufEnd   (End); }
            void                  PutGran           (ORDHW    Pos);
            int                   GetBufferBase     (int      Pos)  const;
            ORDHW                 GetBufferLen      (int      Pos)  const;
            ORDHW                 GranNum           () const { return mGranNum; }
            int                   GapNum            () const { return mPoolReverse. GranBnd () - mPoolNormal. GranBnd () + 1; }
            int                   FreeGranNormal    () const;
            int                   FreeGranReverse   () const;
  };

/************* inlining and templating *******************/

struct MEM3_Arith  // 'struct' because 2.7.2 supports no namespaces
  {
    static
    int
    Sign (int  Num)
      {
        return 1 + 2 * (Num >> (sizeof (int) * 8 - 1));
      }

    static
    bool
    Minus (int  Num)
      {
        return (Num >> (sizeof (int) * 8 - 1)) & 1;
      }
  }; // namespace MEM3_INTERN

inline
int
cFft1Bnd:: DecodeClust (ORD08 const *  CodeArr, int  Step)
  {
    return Decode (CodeArr, Step, eShClustLenStart, eShClustLenBits);
  }

inline
int
cFft1Bnd:: DecodeBlock (ORD08 const *  CodeArr, int  Step)
  {
    return Decode (CodeArr, Step, eShBlockLenStart, eShBlockLenBits);
  }

inline
void
cFft1Bnd:: EncodeClust (ORD08 *  CodeArr, int  Num)
  {
    Encode (CodeArr, Num, eShClustLenStart, eShClustLenBits);
  }

inline
void
cFft1Bnd:: EncodeBlock (ORD08 *  CodeArr, int  Num)
  {
    Encode (CodeArr, Num, eShBlockLenStart, eShBlockLenBits);
  }

inline
int
cFft1Bnd:: GetCode  (ORD08  Code, int  MStart, int  MLen)
  {
    int const  Shift = MStart + 1 - MLen;

    return (int (Code) & (((1 << MLen) - 1) << Shift)) >> Shift;
  }

inline
void
cFft1Bnd:: PutCode  (ORD08 &  Code, int  MStart, int  MLen, int  Num)
  {
    int   const  Shift = MStart + 1 - MLen;
    ORD08 const  Mask  = (1 << MLen) - 1;

    Code &= ~(Mask << Shift);
    Code |= (ORD08 (Num) & Mask) << Shift;
  }

inline
bool
cFft1Bnd:: CluStart (ORD08  Code)
  {
    return (Code & (1 << eShCluStart)) != 0;
  }

inline
bool
cFft1Bnd:: BufStart (ORD08  Code)
  {
    return (Code & (1 << eShBufStart)) != 0;
  }

inline
int
cFft2Bnd::  GetFreeLenNormal  (int  Pos) const
  {
    return mPoolNormal. GetFreeLenBase (Pos);
  }

inline
int
cFft2Bnd::  GetFreeLenReverse (int  End) const
  {
    return mPoolReverse. GetFreeLenBase (End);
  }

inline
int
cFft2Bnd::  GetUsedLenNormal  (int  Pos) const
  {
    return mPoolNormal. GetUsedLenBase (Pos);
  }

inline
int
cFft2Bnd::  GetUsedLenReverse (int  End) const
  {
    return mPoolReverse. GetUsedLenBase (End);
  }

inline
int
cFft2Bnd::  FreeGranNormal () const
  {
    return mPoolReverse. GranBnd () - mPoolNormal. GranUsed () + 1;
  }

inline
int
cFft2Bnd::  FreeGranReverse () const
  {
    return mGranNum - mPoolNormal. GranBnd () - mPoolReverse. GranUsed () + 1;
  }

#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/