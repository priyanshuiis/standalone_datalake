#include <thread>  
#include <chrono>  
#include <cstring>  
#include <vector>  
#include "data_lake.hpp"  
  
// ── Dummy data sizes ───────────────────────────────────────────────────────  
static constexpr int N_UES   = 1;  
static constexpr int N_CELLS = 1;  
static constexpr int N_CBS   = 4;  
static constexpr int TB_SIZE = 1024;  
// IQ buffer: 273*12*14*16 __half2 elements per cell  
static constexpr uint32_t IQ_OFFSET = 273*12*14*16;  
  
// ── Allocate and wire up dummy cuphyPuschDataOut_t ─────────────────────────  
static __half2        dummy_iq[IQ_OFFSET * N_CELLS]{};  
static uint8_t        dummy_tb[TB_SIZE * N_UES]{};  
static uint32_t       dummy_tb_crc[N_UES]{0};          // 0 = CRC pass  
static uint32_t       dummy_cb_crc[N_CBS]{0};  
static uint32_t       dummy_tb_offsets[N_UES]{0};  
static uint32_t       dummy_cb_offsets[N_UES]{0};  
static float          dummy_sinr[N_UES]{20.0f};  
static float          dummy_ta[N_UES]{0.0f};  
static float          dummy_rssi[N_UES]{-80.0f};  
static float          dummy_rsrp[N_UES]{-90.0f};  
static cuFloatComplex dummy_hest[273*12*4*4]{};  
static uint32_t       dummy_hest_sizes[1]{273*12*4*4};  
  
static cuphyPuschDataOut_t make_dummy_out() {  
    cuphyPuschDataOut_t out{};  
    out.pDataRx                = dummy_iq;  
    out.pTbPayloads            = dummy_tb;  
    out.pTbCrcs                = dummy_tb_crc;  
    out.pCbCrcs                = dummy_cb_crc;  
    out.totNumCbs              = N_CBS;  
    out.pStartOffsetsTbPayload = dummy_tb_offsets;  
    out.pStartOffsetsTbCrc     = dummy_tb_offsets;  
    out.pStartOffsetsCbCrc     = dummy_cb_offsets;  
    out.pSinrPostEq            = dummy_sinr;  
    out.pSinrPreEq             = dummy_sinr;  
    out.pTaEsts                = dummy_ta;  
    out.pRssi                  = dummy_rssi;  
    out.pRsrp                  = dummy_rsrp;  
    out.pChannelEsts           = dummy_hest;  
    out.pChannelEstSizes       = dummy_hest_sizes;  
    return out;  
}  
  
// ── Allocate and wire up dummy pusch_params ────────────────────────────────  
static slot_command_api::pusch_params make_dummy_params() {  
    slot_command_api::pusch_params p;  
    p.cell_grp_info.nCells  = N_CELLS;  
    p.cell_grp_info.nUes    = N_UES;  
    p.cell_grp_info.nUeGrps = 1;  
  
    p.cell_dyn_info[0].cellPrmStatIdx = 0;  
    p.cell_dyn_info[0].cellPrmDynIdx  = 0;  
    p.cell_dyn_info[0].slotNum        = 0;  
  
    p.ue_grp_info[0].startPrb      = 0;  
    p.ue_grp_info[0].nPrb          = 106;  
    p.ue_grp_info[0].puschStartSym = 2;  
    p.ue_grp_info[0].nPuschSym     = 12;  
    p.ue_grp_info[0].dmrsSymLocBmsk = 0x0004;  
  
    p.ue_dmrs_info[0].dmrsAddlnPos       = 0;  
    p.ue_dmrs_info[0].nDmrsCdmGrpsNoData = 2;  
    p.ue_dmrs_info[0].dmrsScrmId         = 0;  
  
    p.ue_info[0].rnti           = 0x1234;  
    p.ue_info[0].mcsIndex       = 16;  
    p.ue_info[0].mcsTableIndex  = 0;  
    p.ue_info[0].qamModOrder    = 6;  
    p.ue_info[0].targetCodeRate = 658;  
    p.ue_info[0].nUeLayers      = 1;  
    p.ue_info[0].pduBitmap      = 0x01;  
    p.ue_info[0].pUeGrpPrm     = &p.ue_grp_info[0];  
    p.ue_info[0].ueGrpIdx       = 0;  
    p.ue_info[0].rv             = 0;  
    p.ue_info[0].harqProcessId  = 0;  
    p.ue_info[0].ndi            = 1;  
    p.ue_info[0].pUciPrms       = nullptr;  
  
    p.ue_tb_size[0] = TB_SIZE;  
    return p;  
}  
  
// ── Dummy static params ────────────────────────────────────────────────────  
static cuphyCellStatPrm_t cell_stat{};  
static cuphyPuschStatPrms_t make_dummy_stat() {  
    cell_stat.phyCellId  = 1;  
    cell_stat.nRxAnt     = 4;  
    cell_stat.nRxAntSrs  = 4;  
    cuphyPuschStatPrms_t s{};  
    s.nMaxCells    = N_CELLS;  
    s.pCellStatPrms = &cell_stat;  
    return s;  
}  
  
int main() {  
    DataLake dl(  
        /*enableDbInsert=*/true,  
        /*numSamples=*/1000,  
        /*dbAddress=*/"localhost",  
        /*dbEngine=*/"Memory",  
        /*dataTypes=*/{"fh", "pusch", "hest"},  
        /*storeFailedPdu=*/false,  
        /*numRowsFh=*/120,  
        /*numRowsPusch=*/400,  
        /*numRowsHest=*/200,  
        /*e3AgentEnabled=*/false  
    );  
  
    std::thread worker(waitForLakeData, &dl);  
    worker.detach();  
  
    auto params    = make_dummy_params();  
    auto out       = make_dummy_out();  
    auto stat      = make_dummy_stat();  
    slot_command_api::slot_indication slot_ind;  
  
    for (uint16_t sfn = 0; sfn < 1024; ++sfn) {  
        for (uint16_t slot = 0; slot < 20; ++slot) {  
            slot_ind.sfn_  = sfn;  
            slot_ind.slot_ = slot;  
            params.cell_dyn_info[0].slotNum = slot;  
  
            dl.notify(0, &slot_ind, &params, &out, &stat);  
            std::this_thread::sleep_for(std::chrono::milliseconds(1));  
        }  
    }  
  
    std::this_thread::sleep_for(std::chrono::seconds(5)); // let inserts drain  
    return 0;  
}
