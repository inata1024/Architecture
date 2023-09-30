#include <stdio.h>
#include "shell.h"

void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */
    //读出当前指令
    uint32_t inst = mem_read_32(CURRENT_STATE.PC);

    //解析指令
    //OPCode=0，R型
    uint32_t OPCode = (inst>>26)&(0b111111);
    if(OPCode == 0){
        uint32_t func = inst & 0b111111;
        uint32_t rs = (inst>>21) & (0b11111);
        uint32_t rt = (inst>>16) & (0b11111);
        uint32_t rd = (inst>>11) & (0b11111);
        uint32_t sa = (inst>>6) & (0b11111);

        switch (func){
            //SYSCALL
        case 0b001100:
            uint32_t v0 = CURRENT_STATE.REGS[2];
            if(v0 == 0x0A)
                RUN_BIT = 0;
            break;
            /*----------------arithmetic instrction----------------*/
            //ADDU
            //GPR[rd] ←GPR[rs] + GPR[rt]
        case 0b100001:
            NEXT_STATE.REGS[rd] = (int32_t)CURRENT_STATE.REGS[rs] + (int32_t)CURRENT_STATE.REGS[rt];
            break;

            //ADD
            //GPR[rd] ←GPR[rs] + GPR[rt]
        case 0b100000:
            NEXT_STATE.REGS[rd] = (int32_t)CURRENT_STATE.REGS[rs] + (int32_t)CURRENT_STATE.REGS[rt];
            break;

            //SUB
            //GPR[rd] ←GPR[rs] - GPR[rt]
        case 0b100010:
            NEXT_STATE.REGS[rd] = (int32_t)CURRENT_STATE.REGS[rs] - (int32_t)CURRENT_STATE.REGS[rt];
            break;

            //SUBU
            //GPR[rd] ←GPR[rs] - GPR[rt]
        case 0b100011:
            NEXT_STATE.REGS[rd] = (int32_t)CURRENT_STATE.REGS[rs] - (int32_t)CURRENT_STATE.REGS[rt];
            break;

            //MULT
        case 0b011000:{
            uint64_t temp = (int64_t)(int32_t)CURRENT_STATE.REGS[rs] * (int64_t)(int32_t)CURRENT_STATE.REGS[rt];
            NEXT_STATE.LO = (uint32_t)temp;
            NEXT_STATE.HI = (uint32_t)(temp>>32);
            break;
        }
        
            //DIV
        case 0b011010:{
            NEXT_STATE.LO = (int32_t)CURRENT_STATE.REGS[rs] / (int32_t)CURRENT_STATE.REGS[rt];
            NEXT_STATE.HI = (int32_t)CURRENT_STATE.REGS[rs] % (int32_t)CURRENT_STATE.REGS[rt];
            break;
        }
                
            //DIVU
        case 0b011011:{
            NEXT_STATE.LO = (uint32_t)CURRENT_STATE.REGS[rs] / (uint32_t)CURRENT_STATE.REGS[rt];
            NEXT_STATE.HI = (uint32_t)CURRENT_STATE.REGS[rs] % (uint32_t)CURRENT_STATE.REGS[rt];
            break;
        } 

            //MULTU
        case 0b011001:{
            uint64_t temp = (uint64_t)CURRENT_STATE.REGS[rs] * (uint64_t)CURRENT_STATE.REGS[rt];
            NEXT_STATE.LO = (uint32_t)temp;
            NEXT_STATE.HI = (uint32_t)(temp>>32);
            break;
        }

            //OR
            //GPR[rd] ← GPR[rs] or GPR[rt]
        case 0b100101:
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt];
            break;
        
            //NOR
            //GPR[rd] ← GPR[rs] nor GPR[rt]
        case 0b100111:
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] ^ CURRENT_STATE.REGS[rt];
            break;

            //AND
            //GPR[rd] ← GPR[rs] and GPR[rt]
        case 0b100100:
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] & CURRENT_STATE.REGS[rt];
            break;

            //SLL
            //GPR[rd] ← GPR[rt]31– sa...0 || 0sa
        case 0b000000:
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << sa;
            break;

            //SLLV
            //GPR[rd] ← GPR[rt]31– sa...0 || 0sa
        case 0b000100:
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << (CURRENT_STATE.REGS[rs] & 0x1F);
            break;

            //SRL
            //GPR[rd] ← 0 sa || GPR[rt]31...sa
        case 0b000010:
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> sa;
            break;

            //SRLV
        case 0b000110:
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> (CURRENT_STATE.REGS[rs] & 0x1F);
            break;

            //SRA
            //GPR[rd] ← (GPR[rt]31)sa || GPR[rt] 31...sa
        case 0b000011:
            NEXT_STATE.REGS[rd] = (int)CURRENT_STATE.REGS[rt] >> sa;
            break;

            //SRAV
        case 0b000111:
            NEXT_STATE.REGS[rd] = (int)CURRENT_STATE.REGS[rt] >> (CURRENT_STATE.REGS[rs] & 0x1F);
            break;

            //XOR
            //GPR[rd] ← GPR[rs] xor GPR[rt]
        case 0b100110:
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] ^ CURRENT_STATE.REGS[rt];
            break;

            //SLT
        case 0b101010:
            NEXT_STATE.REGS[rd] = ((int32_t)CURRENT_STATE.REGS[rs] < (int32_t)CURRENT_STATE.REGS[rt]) ? 1 : 0;
            break;

            //SLTU
        case 0b101011:
            NEXT_STATE.REGS[rd] = ((uint32_t)CURRENT_STATE.REGS[rs] < (uint32_t)CURRENT_STATE.REGS[rt]) ? 1 : 0;
            break;


        /*----------------jump/branch instrction----------------*/
            //JR
            // PC ← GPR[rs]
        case 0b001000:
            NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
            NEXT_STATE.PC -= 4;
            break;

            //JALR
            // GPR[rd] ← PC + 4
            // PC ← GPR[rs]
        case 0b001001:
            //rd是可选的
            CURRENT_STATE.REGS[rd == 0?31:rd] = CURRENT_STATE.PC + 4;
            NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
            NEXT_STATE.PC -= 4;
            break;

        /*----------------other instrction----------------*/

            //MFHI
        case 0b010000:
            NEXT_STATE.REGS[rd] = CURRENT_STATE.HI;
            break;

            //MFLO
        case 0b010010:
            NEXT_STATE.REGS[rd] = CURRENT_STATE.LO;
            break;

            //MTHI
        case 0b010001:
            NEXT_STATE.HI = CURRENT_STATE.REGS[rs];
            break;

            //MTLO
        case 0b010011:
            NEXT_STATE.LO = CURRENT_STATE.REGS[rs];
            break;
        default:
            break;
        }


    }
    else{
            uint32_t rs = (inst>>21) & (0b11111);
            uint32_t rt = (inst>>16) & (0b11111);
            int16_t imm16 = inst;//只送低16位
            int imm = imm16;//符号扩展
            uint32_t imm32u = inst & 0xFFFF;//0拓展
            int target = imm << 2;//branch的target

        switch(OPCode){
            /*----------------arithmetic instrction----------------*/
            //addiu
            //GPR [rt] ← GPR[rs] + (immediate15)16 || immediate15...0
        case 0b001001:
            NEXT_STATE.REGS[rt] = (int32_t)CURRENT_STATE.REGS[rs] + imm;
            break;

            //addi
            //GPR [rt] ← GPR[rs] + (immediate15)16 || immediate15...0
            //本lab忽略溢出，addi与addiu等价
        case 0b001000:
            NEXT_STATE.REGS[rt] = (int32_t)CURRENT_STATE.REGS[rs] + imm;
            break;

            //xori
            //GPR[rt] ← GPR[rs] xor (016 || immediate)
        case 0b001110:
            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] ^ imm32u;
            break;

            //andi
            //GPR[rt] ← 016 || (immediate and GPR[rs]15...0)
        case 0b001100:
            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] & imm32u;
            break;

            //ori
            //GPR[rt] ← GPR[rs]31...16 || (immediate or GPR[rs]15...0)
        case 0b001101:
            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] | imm32u;
            break;

            //lui
            //GPR[rt] ← immediate || 016
        case 0b001111:
            NEXT_STATE.REGS[rt] = imm32u << 16;
            break;

            //slti
        case 0b001010:
            NEXT_STATE.REGS[rt] = ((int32_t)CURRENT_STATE.REGS[rs] < imm) ? 1 : 0;
            break;

            //sltiu
            // Considering both quantities as unsigned integers, if rs is less than the sign-extended immediate, the result is set to one
        case 0b001011:
            NEXT_STATE.REGS[rt] = ((uint32_t)CURRENT_STATE.REGS[rs] < (uint32_t)imm) ? 1 : 0;
            break;

        /*----------------jump/branch instrction----------------*/
            //j
            // PC ← PC31...28 || target || 02 
        case 0b000010:{
            uint32_t j_target = inst & 0x3FFFFFF;
            NEXT_STATE.PC = (CURRENT_STATE.PC & 0xF0000000) + (j_target << 2);
            NEXT_STATE.PC -= 4;
            break;
        }

            //jal
            // GPR[31] ← PC + 4
            // PC ← PC31...28 || target || 02 
        case 0b000011:{
            CURRENT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
            uint32_t j_target = inst & 0x3FFFFFF;
            NEXT_STATE.PC = (CURRENT_STATE.PC & 0xF0000000) + (j_target << 2);
            NEXT_STATE.PC -= 4;
            break;
        }

            //bne
            // PC ← PC + (offset15)14 || offset || 02
        case 0b000101:
            if(CURRENT_STATE.REGS[rt] != CURRENT_STATE.REGS[rs]){
                NEXT_STATE.PC = CURRENT_STATE.PC + target;
                NEXT_STATE.PC -= 4;
            }
            break;

            //beq
            // PC ← PC + (offset15)14 || offset || 02
        case 0b000100:
            if(CURRENT_STATE.REGS[rt] == CURRENT_STATE.REGS[rs]){
                NEXT_STATE.PC = CURRENT_STATE.PC + target;
                NEXT_STATE.PC -= 4;
            }
            break;

            //blez
            // PC ← PC + (offset15)14 || offset || 02
        case 0b000110:
            if((CURRENT_STATE.REGS[rs] >> 31) != 0 || CURRENT_STATE.REGS[rs] == 0){
                NEXT_STATE.PC = CURRENT_STATE.PC + target;
                NEXT_STATE.PC -= 4;
            }
            break;

            //bgtz
            // PC ← PC + (offset15)14 || offset || 02
        case 0b000111:
            if((CURRENT_STATE.REGS[rs] >> 31) == 0 && CURRENT_STATE.REGS[rs] != 0){
                NEXT_STATE.PC = CURRENT_STATE.PC + target;
                NEXT_STATE.PC -= 4;
            }
            break;

            //bltz bgez bltzal bgezal
            // PC ← PC + (offset15)14 || offset || 02
        case 0b000001:
            switch (rt)
            {
                //bltz
            case 0:
                if((CURRENT_STATE.REGS[rs] >> 31) != 0){
                    NEXT_STATE.PC = CURRENT_STATE.PC + target;
                    NEXT_STATE.PC -= 4;
                }
                break;

                //bgez
            case 1:
                if((CURRENT_STATE.REGS[rs] >> 31) == 0){
                    NEXT_STATE.PC = CURRENT_STATE.PC + target;
                    NEXT_STATE.PC -= 4;
                }
                break;

                //bltzal
            case 16:
                if((CURRENT_STATE.REGS[rs] >> 31) != 0){
                    CURRENT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
                    NEXT_STATE.PC = CURRENT_STATE.PC + target;
                    NEXT_STATE.PC -= 4;
                }
                break;

                //bgezal
            case 17:
                if((CURRENT_STATE.REGS[rs] >> 31) == 0){
                    CURRENT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
                    NEXT_STATE.PC = CURRENT_STATE.PC + target;
                    NEXT_STATE.PC -= 4;
                }
                break;
            default:
                break;
            }
            break;

            /*----------------mem instruction----------------*/

            //lw
            // vAddr ← ((offset15)16 || offset15...0) + GPR[base]
        case 0b100011:{
            uint32_t virtual_address = CURRENT_STATE.REGS[rs] + imm;
            NEXT_STATE.REGS[rt] = mem_read_32(virtual_address);
            break;
        }

            //sw
            // vAddr ← ((offset15)16 || offset15...0) + GPR[base]
        case 0b101011:{
            uint32_t virtual_address = CURRENT_STATE.REGS[rs] + imm;
            mem_write_32(virtual_address,CURRENT_STATE.REGS[rt]);
            break;
        }

            //sb
        case 0b101000:{
            uint32_t virtual_address = CURRENT_STATE.REGS[rs] + imm;
            uint32_t temp = mem_read_32(virtual_address);
            //拼接temp的高24位与rt的低8位
            temp = (temp | 0xFF) & (CURRENT_STATE.REGS[rt] | 0xFFFFFF00);
            mem_write_32(virtual_address,temp);
            break;
        }

            //lb
        case 0b100000:{
            uint32_t virtual_address = CURRENT_STATE.REGS[rs] + imm;
            uint8_t temp_8 = mem_read_32(virtual_address);
            //sign extend
            int temp = (int8_t)temp_8;
            NEXT_STATE.REGS[rt] = temp;
            break;
        }

            //lbu
        case 0b100100:{
            uint32_t virtual_address = CURRENT_STATE.REGS[rs] + imm;
            //zero extend
            uint32_t temp = mem_read_32(virtual_address) & 0xFF;
            NEXT_STATE.REGS[rt] = temp;
            break;
        }

            //sh
        case 0b101001:{
            uint32_t virtual_address = CURRENT_STATE.REGS[rs] + imm;
            uint32_t temp = mem_read_32(virtual_address);
            //拼接temp的高16位与rt的低16位
            temp = (temp | 0xFFFF) & (CURRENT_STATE.REGS[rt] | 0xFFFF0000);
            mem_write_32(virtual_address,temp);
            break;
        }

            //lh
        case 0b100001:{
            uint32_t virtual_address = CURRENT_STATE.REGS[rs] + imm;
            uint16_t temp_16 = mem_read_32(virtual_address);
            //sign extend
            int temp = temp_16;
            NEXT_STATE.REGS[rt] = temp;
            break;
        }

            //lhu
        case 0b100101:{
            uint32_t virtual_address = CURRENT_STATE.REGS[rs] + imm;
            //zero extend
            uint32_t temp = mem_read_32(virtual_address) & 0xFFFF;
            NEXT_STATE.REGS[rt] = temp;
            break;
        }


        default:
            break;

        }
    }

    //PC自增
    NEXT_STATE.PC += 4;
}
