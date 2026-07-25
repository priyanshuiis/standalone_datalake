#pragma once  
#include <cstdint>  
#include <cstring>  
#include "cuphy_types.h"  
  
static constexpr int MAX_PUSCH_UE_PER_TTI    = 132;  
static constexpr int MAX_PUSCH_UE_GROUPS      = 132;  
static constexpr int MAX_CELLS_PER_CELL_GROUP = 48;  
  
namespace slot_command_api {  
  
struct slot_indication {  
    uint64_t tick_{};  
    uint64_t t0_{};  
    uint16_t sfn_{};  
    uint16_t slot_{};  
    bool     t0_valid_{false};  
    slot_indication() = default;  
    slot_indication(uint16_t sfn, uint16_t slot, uint64_t tick)  
        : sfn_(sfn), slot_(slot), tick_(tick) {}  
};  
  
struct pusch_params {  
    uint32_t                   ue_tb_size[MAX_PUSCH_UE_PER_TTI]{};  
    cuphyPuschCellGrpDynPrm_t  cell_grp_info{};  
    cuphyPuschCellDynPrm_t     cell_dyn_info[MAX_CELLS_PER_CELL_GROUP]{};  
    cuphyPuschUeGrpPrm_t       ue_grp_info[MAX_PUSCH_UE_GROUPS]{};  
    cuphyPuschDmrsPrm_t        ue_dmrs_info[MAX_PUSCH_UE_GROUPS]{};  
    cuphyPuschUePrm_t          ue_info[MAX_PUSCH_UE_PER_TTI]{};  
  
    pusch_params() {  
        // Wire up internal pointers (mirrors original slot_command.hpp constructor)  
        cell_grp_info.pCellPrms  = cell_dyn_info;  
        cell_grp_info.pUeGrpPrms = ue_grp_info;  
        cell_grp_info.pUePrms    = ue_info;  
        for (int i = 0; i < MAX_PUSCH_UE_GROUPS; ++i) {  
            ue_grp_info[i].pDmrsDynPrm = &ue_dmrs_info[i];  
            ue_grp_info[i].pCellPrm    = cell_dyn_info;  
        }  
    }  
};  
  
} // namespace slot_command_api
