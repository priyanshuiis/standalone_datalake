#pragma once  
#include <cstdint>  
  
// ── Half-precision complex (replaces cuda_fp16.h __half2) ──────────────────  
struct __half2 { uint16_t x; uint16_t y; };  
  
// ── Single-precision complex (replaces cuComplex from CUDA) ────────────────  
struct cuFloatComplex { float x; float y; };  
typedef cuFloatComplex float2;  
  
// ── DMRS parameters ────────────────────────────────────────────────────────  
typedef struct {  
    uint8_t  dmrsAddlnPos;  
    uint8_t  dmrsMaxLen;  
    uint8_t  nDmrsCdmGrpsNoData;  
    uint16_t dmrsScrmId;  
} cuphyPuschDmrsPrm_t;  
  
// ── Cell dynamic parameters ────────────────────────────────────────────────  
typedef struct {  
    uint16_t cellPrmStatIdx;  
    uint16_t cellPrmDynIdx;  
    uint16_t slotNum;  
} cuphyPuschCellDynPrm_t;  
  
// ── UE group parameters ────────────────────────────────────────────────────  
typedef struct {  
    cuphyPuschCellDynPrm_t* pCellPrm;  
    cuphyPuschDmrsPrm_t*    pDmrsDynPrm;  
    uint16_t startPrb;  
    uint16_t nPrb;  
    uint8_t  puschStartSym;  
    uint8_t  nPuschSym;  
    uint16_t dmrsSymLocBmsk;  
    uint16_t rssiSymLocBmsk;  
    uint16_t nUes;  
    uint16_t* pUePrmIdxs;   // not used by data_lake but needed for struct layout  
} cuphyPuschUeGrpPrm_t;  
  
// ── Per-UE parameters ──────────────────────────────────────────────────────  
typedef struct {  
    uint16_t pduBitmap;  
    cuphyPuschUeGrpPrm_t* pUeGrpPrm;  
    uint16_t ueGrpIdx;  
    uint8_t  enableTfPrcd;  
    uint32_t puschIdentity;  
    uint8_t  scid;  
    uint16_t dmrsPortBmsk;  
    uint8_t  mcsTableIndex;  
    uint8_t  mcsIndex;  
    uint16_t targetCodeRate;  
    uint8_t  qamModOrder;  
    uint8_t  rv;  
    uint16_t rnti;  
    uint16_t dataScramId;  
    uint8_t  nUeLayers;  
    uint8_t  ndi;  
    uint8_t  harqProcessId;  
    void*    pUciPrms;       // nullptr in standalone  
} cuphyPuschUePrm_t;  
  
// ── Cell group dynamic parameters ─────────────────────────────────────────  
struct cuphyPuschCellGrpDynPrm {  
    uint16_t                nCells;  
    cuphyPuschCellDynPrm_t* pCellPrms;  
    uint16_t                nUeGrps;  
    cuphyPuschUeGrpPrm_t*  pUeGrpPrms;  
    uint16_t                nUes;  
    cuphyPuschUePrm_t*      pUePrms;  
};  
typedef struct cuphyPuschCellGrpDynPrm cuphyPuschCellGrpDynPrm_t;  
  
// ── Cell static parameters ─────────────────────────────────────────────────  
typedef struct {  
    uint16_t phyCellId;  
    uint16_t nRxAnt;  
    uint16_t nRxAntSrs;  
} cuphyCellStatPrm_t;  
  
// ── PUSCH static parameters (only fields used by data_lake) ───────────────  
typedef struct {  
    uint16_t            nMaxCells;  
    cuphyCellStatPrm_t* pCellStatPrms;  
} cuphyPuschStatPrms_t;  
  
// ── PUSCH output data (only fields used by data_lake) ─────────────────────  
typedef struct {  
    __half2*     pDataRx;               // IQ samples (FH data)  
    uint32_t*    pCbCrcs;  
    uint32_t*    pTbCrcs;  
    uint8_t*     pTbPayloads;  
    uint32_t     totNumCbs;  
    uint32_t*    pStartOffsetsCbCrc;  
    uint32_t*    pStartOffsetsTbCrc;  
    uint32_t*    pStartOffsetsTbPayload;  
    float*       pTaEsts;  
    float*       pRssi;  
    float*       pRsrp;  
    float*       pSinrPreEq;  
    float*       pSinrPostEq;  
    cuFloatComplex* pChannelEsts;       // H estimates  
    uint32_t*    pChannelEstSizes;  
} cuphyPuschDataOut_t;
