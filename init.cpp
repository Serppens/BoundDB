/*
    Source Code        BoundDB
    Developed by       Serpens
    
    Copyright (c) 2023-2025
    

    Distributed under the MIT software license, see the accompanying
    file COPYING or http://www.opensource.org/licenses/mit-license.php.
*/
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <vector>
#include <type_traits>
#include <map>
#include <random>
#include <ranges>
#include <unistd.h>

using std::map;
using std::string;

string get_uuid(){
    static std::random_device dev;
    static std::mt19937 rng(dev());

    std::uniform_int_distribution<int> dist(0, 15);

    const char *v = "0123456789abcdef";
    const bool dash[] = { 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0 };

    string res;
    for (int i = 0; i < 16; i++) {
        if (dash[i]) res += "-";
        res += v[dist(rng)];
        res += v[dist(rng)];
    }
    return res;
}

std::string v_check(int c,int &f_c,std::string px){
    int pnty=0;
    std::vector<char> seq0={'{','>','}'};
    std::vector<char> seq1={'{','<','}'};
    int vsy=-1;
    std::string x_y="";
    for (int i = 0; i < px.size(); i++){
        x_y+=px[i];
        if(px[i]==seq0[pnty]||px[i]==seq1[pnty]){
            if(pnty==1){
                if(px[i]==seq0[pnty]){
                    vsy=0;
                }else if(px[i]==seq1[pnty]){
                    vsy=1;
                }
                pnty+=1;
            }else if(pnty==2){
                if(vsy==0){
                    f_c+=1;
                }else if(vsy==1){
                    f_c-=1;
                }
                vsy=-1;
                pnty=0;
            }else{
                pnty+=1;
            }
        }else{
            pnty=0;
            vsy=-1;
        }
        if(c!=0&&f_c==0){
            break;
        }
    }
    return x_y;
}
std::vector<std::string> parseKV(const std::string& prms) {
    std::vector<std::string> xsx;
    std::vector<char>v_0={':','^',':'};
    int p_0=0;
    std::vector<char>v_1={'>','|','<'};
    int p_1=0;
    std::vector<std::string>p_s={"",""};
    int step=0;
    for(int i=0;i<prms.length();i++){
        if(step==0){
            p_s[0]+=prms[i];
            if(prms[i]==v_0[p_0]){
                p_0+=1;
                if(p_0==3){
                    p_s[0]=p_s[0].substr(0,p_s[0].find(":^:"));
                    step=1;
                }
            }
        }else if(step==1){
            p_s[1]+=prms[i];
            if(prms[i]==v_1[p_1]){
                p_1+=1;
                if(p_1==3){
                    p_s[1]=p_s[1].substr(0,p_s[1].find(">|<"));
                    step=2;
                }
            }
        }else if(step==2){
            xsx.push_back(p_s[0]+":^:"+p_s[1]);
            step=0;
            p_0=0;
            p_1=0;
            p_s[0]=prms[i];
            p_s[1]="";
        }
    }
    xsx.push_back(p_s[0]+":^:"+p_s[1]);
    return xsx;
}

std::vector<std::string> parse_dict(const std::string&input){
    std::vector<std::string> result;
    std::stringstream ss(input);
    std::string token;
    while(std::getline(ss, token, ','))result.push_back(token);
    return result;
}
std::vector<std::string> parse_dictV2(const std::string&input){
    bool escp=false;
    std::vector<bool>entr={false,false};
    
    bool er=false;
    bool nx=false;
    if(input[0]!='['||input[input.length()-1]!=']'){
        return{"err"};
    }
    std::vector<std::string> result={};
    std::string vc="";
    for(int i=0;i<input.length();i++){
        if(nx==true){
            if(input[i]!=','&&input[i]!=']'){
                er=true;
                break;
            }else if(input[i]==']'){
                break;
            }
            nx=false;
        }else{
            if(entr[0]==false){
                entr[0]=true;
            }else{
                if(entr[1]==false){
                    if(input[i]!='"'){
                        er=true;
                        break;
                    }
                    entr[1]=true;
                    continue;
                }
                if(input[i]=='\\')
                    escp=true;
                
                if(input[i]=='"'&&escp==false){
                    result.push_back(vc);
                    nx=true;
                    vc="";
                    escp=false;
                    entr[0]=false;
                }else{
                    vc+=input[i];
                    if(escp==true)
                        escp=false;
                }
            }
        }
    }
    if(er==true){
        return{"err"};
    }else{
        return result;
    }
}
std::vector<std::string> parseP(const std::string&input){
    int step=0;
    int point=0;
    std::vector<std::string> result={"",""};
    std::vector<char> seqn={':','^',':'};
    for(int i=0;i<input.length();i++){
        if(step==0){
            result[0]+=input[i];
            if(input[i]==seqn[point]){
                point+=1;
                if(point==3){
                    result[0]=result[0].substr(0,result[0].find(":^:"));
                    step=1;
                }
            }
        }else{
            result[1]+=input[i];
        }
    }
    return result;
}
int lock(){
    while(true){
        if(!std::filesystem::exists("bound/db/tables/lock.bd")){
            std::ofstream outputFile("bound/db/tables/lock.bd");
            outputFile << "";
            outputFile.close();
            break;
        }
        sleep(0.5);
    }
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: bound <command>" << std::endl;
        return 1;
    }
    std::string command = argv[1];
    if(command=="help"){
        std::cout<<"Commands: \n    create_table\n      bound create_table <NameTable> <[identifier1,identifier2...]>\n      "<<std::endl;
    }else if(command=="create_table"){
        if(argc<4){
            std::cout << "Usage: bound create_table <NameTable> <[name_identifier0...]>" << std::endl;
            return -1;
        }
        lock();
        std::string arg = argv[3];
        std::vector<std::string>arr=parse_dict(arg);
        arr[0].erase(std::remove(arr[0].begin(), arr[0].end(), '['), arr[0].end());
        arr[arr.size()-1].erase(std::remove(arr[arr.size()-1].begin(), arr[arr.size()-1].end(), ']'), arr[arr.size()-1].end());
        std::stringstream wlt;
        for(int i=0;i<arr.size();i++){
            wlt<<arr[i]<<"<!>";
        }
        std::string nameTable=argv[2];
        if(!std::filesystem::exists("bound/db/tables/")){
            std::filesystem::create_directory("bound/db/tables/");
        }
        if(std::filesystem::exists("bound/db/tables/"+nameTable+"/config.bd")){
            std::cout << "This table already exists"<<std::endl;
            return 1;
        }
        std::filesystem::create_directory("bound/db/tables/"+nameTable);

        std::stringstream nw;
        nw<<"{\"currensm\":\"md201\"<|>\"qty\":\"0\"<|>\"indza\":\"0\"<|>\"sizeKb\":\"0\"<|>\"identifier\":";
        nw<<wlt.str();
        nw<<"<|>}";
        std::ofstream outputFile("bound/db/tables/"+nameTable+"/config.bd");
        outputFile << nw.str();
        outputFile.close();
        remove("bound/db/tables/lock.bd");
        return 0;
    }
    else if (command=="insert") {
        if(argc<3){
            std::cout << "Usage: bound insert table_name [identifier=value,column0=value...]" << std::endl;
            return -1;
        }
        std::string tableName=argv[2];
        if(!std::filesystem::exists("bound/db/tables/"+tableName)){
            std::cout << "This table not exist" << std::endl;
            return -1;
        }
        if(std::filesystem::exists("bound/db/tables/"+tableName+"/insert.bd")){
            lock();
            std::ifstream conf("bound/db/tables/"+tableName+"/insert.bd");
            std::stringstream *by = new std::stringstream;
            (*by)<<conf.rdbuf();
            conf.close();
            std::string arg=(*by).str();
            by->str(std::string());
            std::string currensm;
            int qty;
            int currentBd;
            int indza;
            int sizeKb;
            std::ifstream inputFile("bound/db/tables/"+tableName+"/config.bd");
            std::stringstream buffer;
            buffer << inputFile.rdbuf();
            std::string fileContent = buffer.str();
            inputFile.close();
            size_t startPos=fileContent.find(":",fileContent.find("currensm"));        
            size_t endPos=fileContent.find("<|>",startPos);
            size_t startPos0=fileContent.find(":",fileContent.find("qty"));        
            size_t endPos0=fileContent.find("<|>",startPos0);
            size_t startPos1=fileContent.find(":",fileContent.find("indza"));        
            size_t endPos1=fileContent.find("<|>",startPos1);
            size_t startPos2=fileContent.find(":",fileContent.find("sizeKb"));        
            size_t endPos2=fileContent.find("<|>",startPos2);
            currensm=fileContent.substr(startPos+1,endPos-1-startPos);
            currensm.erase(std::remove(currensm.begin(), currensm.end(), '"'), currensm.end());
            std::string qtybefore=fileContent.substr(startPos0+1,endPos0-1-startPos0);
            qtybefore.erase(std::remove(qtybefore.begin(), qtybefore.end(), '"'), qtybefore.end());
            qty=stoi(qtybefore);
            std::string indzabef=fileContent.substr(startPos1+1,endPos1-1-startPos1);
            indzabef.erase(std::remove(indzabef.begin(), indzabef.end(), '"'), indzabef.end());
            indza=stoi(indzabef);
            std::string kbbefore=fileContent.substr(startPos2+1,endPos2-1-startPos2);
            kbbefore.erase(std::remove(kbbefore.begin(), kbbefore.end(), '"'), kbbefore.end());
            sizeKb=stoi(kbbefore);

            size_t colsize=fileContent.find("identifier");
            std::string col=fileContent.substr(colsize);
            size_t colend=fileContent.find("}",colsize);
            std::string cld=fileContent.substr(colsize+12,(colend-12)-colsize-3);
            std::vector<std::string> dict=parseKV(arg);
            size_t fr=cld.find(",");
            cld=cld.substr(0,fr);
            bool exist=false;
            std::stringstream felx;
            std::vector<std::string> cxd;
            for (const auto& arg : dict) {
                std::vector<std::string>params=parseP(arg);
                std::string cldtmp=cld;
                while(true){
                    if(cldtmp.find("<!>")==std::string::npos){
                        break;
                    }
                    if(params[0]==cldtmp.substr(0,cldtmp.find("<!>"))){
                        cxd.push_back(cldtmp.substr(0,cldtmp.find("<!>")));
                        exist=true;
                        break;
                    }
                    felx<<cldtmp.substr(0,cldtmp.find("<!>"))+", ";
                    cldtmp=cldtmp.substr(cldtmp.find("<!>")+3);
                }
            }
            if(!exist){
                std::cout<<"You need insert identifier: "+felx.str()<<std::endl;
                std::string wis="bound/db/tables/"+tableName+"/insert.bd";
                remove(wis.c_str());
                remove("bound/db/tables/lock.bd");
                return -1;
            }
            std::stringstream s;
            std::stringstream ss;
            if(indza==10){
                indza=0;
                if(qty==10){
                    while(true){
                        const std::string validCharacters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*";
                        std::random_device rd;
                        std::mt19937 gen(rd());
                        std::uniform_int_distribution<int> dist(0, validCharacters.size() - 1);
                        std::string generatedPassword;
                        for (int i = 0; i < 6; ++i) {
                            int charIndex = dist(gen);
                            generatedPassword += validCharacters[charIndex];
                        }
                        std::string directoryPath = "bound/db/tables/"+tableName+"/"+generatedPassword;
                        if (!std::filesystem::exists(directoryPath)) {
                            std::filesystem::create_directory(directoryPath);
                            currensm=generatedPassword;
                            break;
                        }
                    }
                    qty=0;
                }else{
                    qty=qty+1;
                }
            }
            if(!std::filesystem::exists("bound/db/tables/"+tableName+"/"+currensm)){
                ss<<"<|>";
                std::filesystem::create_directory("bound/db/tables/"+tableName+"/"+currensm);
            }else if(std::filesystem::exists("bound/db/tables/"+tableName+"/"+currensm+"/"+std::to_string(qty)+".bd")){
                std::string cnt;
                std::ifstream ifl("bound/db/tables/"+tableName+"/"+currensm+"/"+std::to_string(qty)+".bd");
                std::stringstream bff;
                bff << ifl.rdbuf();
                cnt=bff.str();
                ifl.close();
                ss<<cnt;
            }else{
                ss<<"<|>";
            }
            std::string cont;
            if(std::filesystem::exists("bound/db/tables/"+tableName+"/find.bd")){
                std::ifstream inputFile("bound/db/tables/"+tableName+"/find.bd");
                std::stringstream buffer;
                buffer << inputFile.rdbuf();
                cont = buffer.str();
                inputFile.close();
                std::string conttmp=cont;
                s<<cont;
            }else{
                s<<"<|>";
            }
            s<<currensm;
            s<<">|<";
            s<<qty;
            s<<">|<";
            s<<indza;
            s<<">|<";
            for (const auto& arg : dict) {
                size_t endPos=arg.find(":^:");
                if(endPos==std::string::npos||arg.substr(endPos)==":^:"){
                    std::cout << "Usage: bound insert table_name [column0:^:value^|^column1:^:value...]0" << std::endl;
                    std::string wis="bound/db/tables/"+tableName+"/insert.bd";
                    remove(wis.c_str());
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }
                std::string col0=col;
                std::vector<std::string>params=parseP(arg);
                for(int i=0;i<cxd.size();i++){
                    if(params[0]==cxd[i]){
                        size_t cn=cont.find(params[1]);
                        std::string conttmp=cont;
                        while(true){
                            size_t cn=conttmp.find(params[1]);
                            if(cn!=std::string::npos){
                                size_t cnend=conttmp.find("<!>",cn);
                                std::string f=conttmp.substr(cn,cnend-cn);
                                char ax=conttmp.substr(cn-1,cnend-cn)[0];
                                std::string ff=conttmp.substr(cn);
                                size_t cdn=ff.find("<!>");
                                ff=ff.substr(cdn);
                                if(f==params[1]&&(stoi(std::to_string(ax))==62||stoi(std::to_string(ax))==60)){
                                    std::cout<<"This identification already exists"<<std::endl;
                                    std::string wis="bound/db/tables/"+tableName+"/insert.bd";
                                    remove(wis.c_str());
                                    remove("bound/db/tables/lock.bd");
                                    return -1;
                                }
                                conttmp=ff;
                                continue;
                            }
                            break;
                        }

                        s<<params[1];
                        s<<"<!>";
                    }
                }
                if(dict.back()==arg){
                    indza=indza+1;
                    ss<<arg;
                    ss<<"<|>";
                }else{
                    ss<<arg;
                    ss<<">|<";
                }
            }
            s<<"<|>";
            std::ofstream xzx("bound/db/tables/"+tableName+"/find.bd");
            xzx<<s.str();
            xzx.close();
            std::stringstream xnw;
            xnw<<"{\"currensm\":\""+currensm+"\"<|>\"qty\":\""+std::to_string(qty)+"\"<|>\"indza\":\""+std::to_string(indza)+"\"<|>\"sizeKb\":\"0\"<|>\"identifier\":";
            xnw<<cld;
            xnw<<"<|>}";
            std::ofstream xcx("bound/db/tables/"+tableName+"/config.bd");
            xcx<<xnw.str();
            xcx.close();
            std::ofstream outputFile("bound/db/tables/"+tableName+"/"+currensm+"/"+std::to_string(qty)+".bd");
            outputFile << ss.str();
            outputFile.close();
            std::string wis="bound/db/tables/"+tableName+"/insert.bd";
            remove(wis.c_str());
            remove("bound/db/tables/lock.bd");
            return 0;
        }
    }else if(command=="findMultiple"){
        if(argc<4){
            std::cout << "Usage: bound findMultiple table [identification0,identification1...]" << std::endl;
            return 1;
        }

        std::string nv=argv[3];
        std::vector<std::string>sdf=parse_dictV2(nv);
        if(sdf[0]=="err"){
            std::cout<<"Error formatter"<<std::endl;
            return -1;
        }
        std::vector<std::string>xvi={};
        std::string tableName=argv[2];
        if(!std::filesystem::exists("bound/db/tables/"+tableName+"/find.bd")){
            std::cout<<"This identification not exists"<<std::endl;
            return 1;
        }
        lock();
        std::ifstream aaq("bound/db/tables/"+tableName+"/find.bd");
        std::stringstream buffer;
        buffer << aaq.rdbuf();
        std::string prty = buffer.str();
        aaq.close();
        bool gmj=false;
        std::string hllwd="";
        for(int i=0;i<sdf.size();i++){
            bool exs=false;
            for(int x=0;x<xvi.size();x++){
                if(xvi[x]==sdf[i]){
                    exs=true;
                    break;
                }
            }
            if(exs!=true){
                std::stringstream ar;
                ar<<sdf[i];
                ar<<"<!>";
                size_t nt=prty.find(ar.str());

                if(nt!=std::string::npos){
                    std::string conttmp=prty;
                    while(true){
                        size_t cn=conttmp.find(ar.str());
                        if(cn!=std::string::npos){
                            std::string se=conttmp.substr(cn-3);
                            se=se.substr(0,3);
                            std::string fr=conttmp.substr(cn);
                            size_t mrn=fr.find("<!>");
                            if(se!=">|<"&&se!="<!>"){
                                conttmp=fr.substr(mrn+3);
                                continue;

                            }
                            if(fr.substr(0,mrn)!=sdf[i]){
                                conttmp=fr.substr(mrn+3);
                                continue;
                            }

                            while(true){
                                int sz=0;
                                int point=0;
                                std::vector<char> seqn={'>','|','<'};
                                while(true){
                                    if(seqn[point]==conttmp.substr(cn-sz)[0]){
                                        point=point+1;
                                        if(point==3){
                                            conttmp=conttmp.substr(cn-sz);
                                            break;
                                        }
                                    }else{
                                        point=0;
                                    }
                                    sz=sz+1;
                                }
                                conttmp=conttmp.substr(3);
                                conttmp=conttmp.substr(0,conttmp.find("<|>"));
                                break;
                            }
                            std::string currensm="";
                            std::string path;
                            std::string ind;
                            for(int spr=0;spr<3;spr++){
                                size_t xnx=conttmp.find(">|<");
                                if(spr==0){
                                    size_t xnx=conttmp.find(">|<");
                                    currensm=conttmp.substr(0,xnx);
                                    conttmp=conttmp.substr(xnx+3);
                                }else if(spr==1){
                                    size_t xnx=conttmp.find(">|<");
                                    path=conttmp.substr(0,xnx);
                                    conttmp=conttmp.substr(xnx+3);
                                }else{
                                    size_t xnx=conttmp.find(">|<");
                                    ind=conttmp.substr(0,xnx);
                                    conttmp=conttmp.substr(xnx+3);
                                }
                            }
                            std::ifstream ymn("bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                            std::stringstream buffer;
                            buffer << ymn.rdbuf();
                            std::string invr = buffer.str();
                            ymn.close();
                            invr=invr.substr(invr.find("<|>")+3);
                            if(stoi(ind)!=0){
                                for(int i=0;i<stoi(ind);i++){
                                    invr=invr.substr(invr.find("<|>")+3);
                                }
                            }
                            std::stringstream xs;
                            invr=invr.substr(0,invr.find("<|>"));
                            while(true){
                                size_t dm=invr.find(">|<");
                                if(dm==std::string::npos){
                                    xs<<invr;
                                    break;
                                }else{
                                    xs<<invr.substr(0,dm)+">|<";
                                    invr=invr.substr(dm+3);
                                }
                            }
                            if(gmj==true){
                                hllwd+=">>>splt<<<";
                            }else{
                                gmj=true;
                            }
                            hllwd+=xs.str();
                            break;
                        }else{
                            std::cout<<"This identification not exists"<<std::endl;
                            remove("bound/db/tables/lock.bd");
                            return -1;
                        }
                    }
                }
            }
        }
        std::cout<<hllwd<<std::endl;
        remove("bound/db/tables/lock.bd");
        return -1;
    }else if(command=="find"){
        if(argc<4){
            std::cout << "Usage: bound find table identification" << std::endl;
            return 1;
        }
        std::string tableName=argv[2];
        if(!std::filesystem::exists("bound/db/tables/"+tableName+"/find.bd")){
            std::cout<<"This identification not exists"<<std::endl;
            return 1;
        }
        lock();
        std::ifstream aaq("bound/db/tables/"+tableName+"/find.bd");
        std::stringstream buffer;
        buffer << aaq.rdbuf();
        std::string prty = buffer.str();
        aaq.close();
        std::stringstream ar;
        ar<<argv[3];
        ar<<"<!>";
        size_t nt=prty.find(ar.str());
        if(nt==std::string::npos){
            std::cout<<"This identification not exists"<<std::endl;
            remove("bound/db/tables/lock.bd");
            return -1;
        }else{
            std::string conttmp=prty;
            while(true){
                size_t cn=conttmp.find(ar.str());
                if(cn==std::string::npos){
                    std::cout<<"This identification not exists"<<std::endl;
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }
                std::string se=conttmp.substr(cn-3);
                se=se.substr(0,3);
                std::string fr=conttmp.substr(cn);
                size_t mrn=fr.find("<!>");
                if(se!=">|<"&&se!="<!>"){
                    conttmp=fr.substr(mrn+3);
                    continue;

                }
                if(fr.substr(0,mrn)!=argv[3]){
                    conttmp=fr.substr(mrn+3);
                    continue;
                }
                while(true){
                    int sz=0;
                    int point=0;
                    std::vector<char> seqn={'>','|','<'};
                    while(true){
                        if(seqn[point]==conttmp.substr(cn-sz)[0]){
                            point=point+1;
                            if(point==3){
                                conttmp=conttmp.substr(cn-sz);
                                break;
                            }
                        }else{
                            point=0;
                        }
                        sz=sz+1;
                    }
                    conttmp=conttmp.substr(3);
                    conttmp=conttmp.substr(0,conttmp.find("<|>"));
                    break;
                }
                std::string currensm="";
                std::string path;
                std::string ind;
                for(int spr=0;spr<3;spr++){
                    size_t xnx=conttmp.find(">|<");
                    if(spr==0){
                        size_t xnx=conttmp.find(">|<");
                        currensm=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }else if(spr==1){
                        size_t xnx=conttmp.find(">|<");
                        path=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }else{
                        size_t xnx=conttmp.find(">|<");
                        ind=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }
                }
                std::ifstream ymn("bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                std::stringstream buffer;
                buffer << ymn.rdbuf();
                std::string invr = buffer.str();
                ymn.close();
                invr=invr.substr(invr.find("<|>")+3);
                if(stoi(ind)!=0){
                    for(int i=0;i<stoi(ind);i++){
                        invr=invr.substr(invr.find("<|>")+3);
                    }
                }
                std::stringstream xs;
                invr=invr.substr(0,invr.find("<|>"));
                while(true){
                    size_t dm=invr.find(">|<");
                    if(dm==std::string::npos){
                        xs<<invr;
                        break;
                    }else{
                        xs<<invr.substr(0,dm)+">|<";
                        invr=invr.substr(dm+3);
                    }
                }
                std::cout<<xs.str()<<std::endl;
                remove("bound/db/tables/lock.bd");
                return 0;
            }
            std::cout<<"This identification not exists"<<std::endl;
            remove("bound/db/tables/lock.bd");
            return -1;
        }
    }else if(command=="add_param_array"){
        if(argc!=6){
            std::cout << "Usage: bound add_param_array <NameTable> <Identification> <name_column_array> <user:^:DarkSouls_23>|<comment:^:Hello>" << std::endl;
            return -1;
        }
        std::string tableName=argv[2];
        std::string tb=argv[3];
        if(tableName.find("<|>")!=std::string::npos||tableName.find(">|<")!=std::string::npos||tableName.find("<!>")!=std::string::npos||
        tb.find("<|>")!=std::string::npos||tb.find(">|<")!=std::string::npos||tb.find("<!>")!=std::string::npos){
            return -1;
        }
        if(!std::filesystem::exists("bound/db/tables/"+tableName)){
            std::cout << "This table not exist" << std::endl;
            return -1;
        }
        lock();
        
        std::ifstream aaq("bound/db/tables/"+tableName+"/find.bd");
        std::stringstream buffer;
        buffer << aaq.rdbuf();
        std::string prty = buffer.str();
        aaq.close();
        std::ifstream appendFile("bound/db/tables/"+tableName+"/append.bd");

        std::stringstream ar;
        ar<<argv[3];
        ar<<"<!>";
        size_t nt=prty.find(ar.str());
        if(nt==std::string::npos){
            std::cout<<"This identification not exists"<<std::endl;
            std::string wis="bound/db/tables/"+tableName+"/append.bd";
            remove(wis.c_str());
            remove("bound/db/tables/lock.bd");
            return -1;
        }else{
            std::string conttmp=prty;
            while(true){
                size_t cn=conttmp.find(ar.str());
                if(cn==std::string::npos){
                    std::cout<<"This identification not exists"<<std::endl;
                    std::string wis="bound/db/tables/"+tableName+"/append.bd";
                    remove(wis.c_str());
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }
                std::string se=conttmp.substr(cn-3);
                se=se.substr(0,3);
                std::string fr=conttmp.substr(cn);
                size_t mrn=fr.find("<!>");
                if(se!=">|<"&&se!="<!>"){
                    conttmp=fr.substr(mrn+3);
                    continue;
                }
                if(fr.substr(0,mrn)!=argv[3]){
                    conttmp=fr.substr(mrn+3);
                    continue;
                }
                while(true){
                    int sz=0;
                    int point=0;
                    std::vector<char> seqn={'>','|','<'};
                    while(true){
                        if(seqn[point]==conttmp.substr(cn-sz)[0]){
                            point=point+1;
                            if(point==3){
                                conttmp=conttmp.substr(cn-sz);
                                break;
                            }
                        }else{
                            point=0;
                        }
                        sz=sz+1;
                    }
                    conttmp=conttmp.substr(3);
                    conttmp=conttmp.substr(0,conttmp.find("<|>"));
                    break;
                }
                std::string currensm="";
                std::string path;
                std::string ind;
                for(int spr=0;spr<3;spr++){
                    size_t xnx=conttmp.find(">|<");
                    if(spr==0){
                        size_t xnx=conttmp.find(">|<");
                        currensm=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }else if(spr==1){
                        size_t xnx=conttmp.find(">|<");
                        path=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }else{
                        size_t xnx=conttmp.find(">|<");
                        ind=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }
                }
                std::ifstream ymn("bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                std::stringstream xbuffer;
                xbuffer << ymn.rdbuf();
                std::string invr = xbuffer.str();
                ymn.close();
                std::stringstream bfr;
                bfr<<invr.substr(0,invr.find("<|>")+3);
                invr=invr.substr(invr.find("<|>")+3);
                if(stoi(ind)!=0){
                    for(int i=0;i<stoi(ind);i++){
                        bfr<<invr.substr(0,invr.find("<|>")+3);
                        invr=invr.substr(invr.find("<|>")+3);
                    }
                }
                std::string nvvc=argv[5];
                size_t rs=nvvc.find(":^:");
                if(rs==std::string::npos){
                    std::cout<<"Your parameters can't be empty"<<std::endl;
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }
                std::stringstream xs;
                std::string afr=invr.substr(invr.find("<|>"));
                invr=invr.substr(0,invr.find("<|>"));
                std::string uuid=get_uuid();
                if(invr.find(argv[4])==std::string::npos){
                    std::stringstream nxw;
                    nxw<<argv[4]<<"!@!base:^:0>|<uuid_base:^:"<<uuid<<">|<"<<argv[5]<<"^<>!#!";
                    std::stringstream nwimp;
                    nwimp<<invr<<">|<"<<nxw.str();
                    std::string out=bfr.str()+nwimp.str()+afr;
                    std::ofstream outputFile("bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                    outputFile << out;
                    outputFile.close();
                    std::cout<<uuid<<std::endl; 
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }
                size_t arr=invr.find(argv[4]);
                std::string tmpchild=invr.substr(arr);
                std::stringstream nxcheck;
                nxcheck<<argv[4]<<"!@!";
                if(tmpchild.find(nxcheck.str())==std::string::npos){
                    std::stringstream nxw;
                    nxw<<argv[4]<<"!@!base:^:0>|<uuid_base:^:"<<uuid<<">|<"<<argv[5]<<"^<>!#!";
                    std::stringstream nwimp;
                    nwimp<<invr<<">|<"<<nxw.str();
                    std::string out=bfr.str()+nwimp.str()+afr;
                    std::ofstream outputFile("bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                    outputFile << out;
                    outputFile.close();
                    std::cout<<uuid<<std::endl;
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }
                tmpchild=tmpchild.substr(0,tmpchild.find("!#!"));
                std::string tmploop=tmpchild;
                int baseN=0;
                if(tmpchild.find("^<>")!=std::string::npos){
                    while(true){
                        tmploop=tmploop.substr(tmploop.find("^<>")+3);
                        baseN=baseN+1;
                        if(tmploop==""){
                            break;
                        }
                    }
                }
                
                std::stringstream xnw;
                xnw<<"base:^:"<<std::to_string(baseN)<<">|<uuid_base:^:"<<uuid<<">|<"<<argv[5]<<"^<>!#!";
                std::string bfrinvr=invr.substr(0,invr.find(tmpchild));
                std::string afrinvr=invr.substr(invr.find(tmpchild)+tmpchild.length()+3);
                std::ofstream outputFile("bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                outputFile << bfr.str()<<bfrinvr<<tmpchild<<xnw.str()<<afrinvr<<afr;
                outputFile.close();
                remove("bound/db/tables/lock.bd");
                std::cout<<uuid<<std::endl;
                return 0;
            }
        }
        return 0;
    }else if(command=="add_param_child"){
        if(argc!=5){
            std::cout << "Usage: bound add_param_child <NameTable> <Identification> <name_column_array0,name_column_array1...>" << std::endl;
            return -1;
        }
        std::string tableName=argv[2];
        std::string tb=argv[3];
        if(tableName.find("<|>")!=std::string::npos||tableName.find(">|<")!=std::string::npos||tableName.find("<!>")!=std::string::npos||
        tb.find("<|>")!=std::string::npos||tb.find(">|<")!=std::string::npos||tb.find("<!>")!=std::string::npos){
            return -1;
        }
        if(!std::filesystem::exists("bound/db/tables/"+tableName)){
            std::cout << "This table not exist" << std::endl;
            return -1;
        }
        lock();
        
        std::ifstream aaq("bound/db/tables/"+tableName+"/find.bd");
        std::stringstream buffer;
        buffer << aaq.rdbuf();
        std::string prty = buffer.str();
        aaq.close();
        std::ifstream appendFile("bound/db/tables/"+tableName+"/append.bd");

        std::stringstream ar;
        ar<<argv[3];
        ar<<"<!>";
        size_t nt=prty.find(ar.str());
        if(nt==std::string::npos){
            std::cout<<"This identification not exists"<<std::endl;
            std::string wis="bound/db/tables/"+tableName+"/append.bd";
            remove(wis.c_str());
            remove("bound/db/tables/lock.bd");
            return -1;
        }else{
            std::string conttmp=prty;
            while(true){
                size_t cn=conttmp.find(ar.str());
                if(cn==std::string::npos){
                    std::cout<<"This identification not exists"<<std::endl;
                    std::string wis="bound/db/tables/"+tableName+"/append.bd";
                    remove(wis.c_str());
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }
                std::string se=conttmp.substr(cn-3);
                se=se.substr(0,3);
                std::string fr=conttmp.substr(cn);
                size_t mrn=fr.find("<!>");
                if(se!=">|<"&&se!="<!>"){
                    conttmp=fr.substr(mrn+3);
                    continue;
                }
                if(fr.substr(0,mrn)!=argv[3]){
                    conttmp=fr.substr(mrn+3);
                    continue;
                }
                while(true){
                    int sz=0;
                    int point=0;
                    std::vector<char> seqn={'>','|','<'};
                    while(true){
                        if(seqn[point]==conttmp.substr(cn-sz)[0]){
                            point=point+1;
                            if(point==3){
                                conttmp=conttmp.substr(cn-sz);
                                break;
                            }
                        }else{
                            point=0;
                        }
                        sz=sz+1;
                    }
                    conttmp=conttmp.substr(3);
                    conttmp=conttmp.substr(0,conttmp.find("<|>"));
                    break;
                }
                std::string currensm="";
                std::string path;
                std::string ind;
                for(int spr=0;spr<3;spr++){
                    size_t xnx=conttmp.find(">|<");
                    if(spr==0){
                        size_t xnx=conttmp.find(">|<");
                        currensm=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }else if(spr==1){
                        size_t xnx=conttmp.find(">|<");
                        path=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }else{
                        size_t xnx=conttmp.find(">|<");
                        ind=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }
                }
                std::ifstream ymn("bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                std::stringstream xbuffer;
                xbuffer << ymn.rdbuf();
                std::string invr = xbuffer.str();
                ymn.close();
                std::stringstream bfr;
                
                bfr<<invr.substr(0,invr.find("<|>")+3);
                
                invr=invr.substr(invr.find("<|>")+3);
                if(stoi(ind)!=0){
                    for(int i=0;i<stoi(ind);i++){
                        bfr<<invr.substr(0,invr.find("<|>")+3);
                        invr=invr.substr(invr.find("<|>")+3);
                    }
                }
                std::string afr=invr.substr(invr.find("<|>"));
                invr=invr.substr(0,invr.find("<|>"));
                std::string tl=argv[4];
                std::vector<std::string>arr=parse_dict(tl);
                std::string rlp=invr;
                std::string nwp="";
                std::string bfrl="";
                int wn=0;
                int n_w=0;
                int qt=0;

                for(int i=0;i<arr.size();i++){
                    while(true){
                        if(rlp.find(arr[i]+"{>}")!=std::string::npos){
                            if((rlp.substr(rlp.find(arr[i]+"{>}")-3).substr(0,3)==">|<"||rlp.substr(rlp.find(arr[i]+"{>}")-3).substr(0,3)=="<|>")&&(n_w==0||n_w==1)){
                                std::string prev=rlp.substr(0,rlp.find(arr[i]+"{>}"));
                                int f_c=0;
                                v_check(0,f_c,prev);
                                if(f_c!=0){
                                    std::string chn=rlp.substr(prev.size());
                                    std::string gw=v_check(1,f_c,chn);
                                    std::string r_r=prev+gw;
                                    bfrl+=rlp.substr(0,r_r.size());
                                    rlp=rlp.substr(rlp.find(r_r)+r_r.size());
                                    continue;
                                }
                                bfrl+=rlp.substr(0,rlp.find(arr[i]+"{>}")+arr[i].length()+3);
                                rlp=rlp.substr(rlp.find(arr[i]+"{>}")+arr[i].length()+3);
                                if(rlp.substr(0,3)=="y_n"&&rlp.substr(6,7)[0]=='y'){
                                    std::cout<<"You can't create child in this param: "<<arr[i]<<std::endl;
                                    remove("bound/db/tables/lock.bd");
                                    return -1;
                                }
                                n_w=1;
                                qt+=1;
                                int q_y=1;
                                rlp=v_check(2,q_y,rlp);
                            }else if(rlp.substr(rlp.find(arr[i]+"{>}")-3).substr(0,3)!=">|<"||rlp.substr(rlp.find(arr[i]+"{>}")-3).substr(0,3)!="<|>"){
                                std::string prev=rlp.substr(0,rlp.find(arr[i]+"{>}"));
                                int f_c=0;
                                
                                v_check(0,f_c,prev);
                                if(f_c!=0){
                                    std::string chn=rlp.substr(prev.size());
                                    std::string gw=v_check(1,f_c,chn);
                                    std::string r_r=prev+gw;
                                    bfrl+=rlp.substr(0,r_r.size());
                                    rlp=rlp.substr(rlp.find(r_r)+r_r.size());
                                    continue;
                                }
                                std::string chn=rlp.substr(prev.size());
                                std::string r_r=prev+chn.substr(0,chn.find("{<}")+3);
                                bfrl+=rlp.substr(0,r_r.size());
                                rlp=rlp.substr(rlp.find(r_r)+r_r.size());
                                continue;
                            }
                        }
                        else{
                            if(n_w==0){
                                n_w=-1;
                            }else if(n_w!=2&&n_w!=-1){
                                n_w=2;
                            }
                            
                            if(nwp!=""){
                                std::string bfr=nwp.substr(0,nwp.find(arr[i-1]));
                                std::string ins=nwp.substr(nwp.find(arr[i-1]));
                                std::string end=ins.substr(0,ins.find("{<}"));
                                std::string uuid=get_uuid();
                                end+="y_n:^:n>|<uuid_base:^:"+uuid+">|<"+arr[i]+"{>}{<}";
                                nwp=bfr+end;
                                wn+=1;
                            }else{
                                nwp=arr[i]+"{>}{<}";
                            }
                        }
                        break;
                    }
                }
                for(int i=0;i<wn;i++){
                    nwp+="^<>{<}";
                }
                std::string end=bfrl+rlp;
                std::stringstream vcn;
                bool xz=false;
                std::cout<<rlp<<std::endl;
                if(rlp.size()>=6&&rlp.substr(rlp.size()-6)=="^<>{<}"){
                    xz=true;
                }
                vcn<<bfrl;
                if(n_w==-1){
                    vcn<<rlp<<">|<"<<nwp;
                }else{
                    if(xz){
                        vcn<<rlp.substr(0,rlp.size()-6)<<">|<"<<nwp<<"^<>{<}";
                    }else{
                        vcn<<rlp.substr(0,rlp.size()-3)<<"y_n:^:n"<<">|<"<<nwp<<"^<>{<}";
                    }
                }
                vcn<<invr.substr(end.size());
                std::ofstream outputFile("bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                outputFile << bfr.str()<<vcn.str()<<afr;
                outputFile.close();
                remove("bound/db/tables/lock.bd");
                return 1;
            }
        }
    }else if(command=="add_content_child"){
        if(argc!=6){
            std::cout << "Usage: bound add_content_child <NameTable> <Identification> <name_column_array0,name_column_array1...> <user:^:DarkSouls_23>|<comment:^:Hello>" << std::endl;
            return -1;
        }
        
        std::string tableName=argv[2];
        std::string tb=argv[3];
        if(tableName.find("<|>")!=std::string::npos||tableName.find(">|<")!=std::string::npos||tableName.find("<!>")!=std::string::npos||
        tb.find("<|>")!=std::string::npos||tb.find(">|<")!=std::string::npos||tb.find("<!>")!=std::string::npos){
            return -1;
        }
        if(!std::filesystem::exists("bound/db/tables/"+tableName)){
            std::cout << "This table not exist" << std::endl;
            return -1;
        }
        lock();
        std::ifstream aaq("bound/db/tables/"+tableName+"/find.bd");
        std::stringstream buffer;
        buffer << aaq.rdbuf();
        std::string prty = buffer.str();
        aaq.close();
        std::ifstream appendFile("bound/db/tables/"+tableName+"/append.bd");

        std::stringstream ar;
        ar<<argv[3];
        ar<<"<!>";
        size_t nt=prty.find(ar.str());
        if(nt==std::string::npos){
            std::cout<<"This identification not exists"<<std::endl;
            std::string wis="bound/db/tables/"+tableName+"/append.bd";
            remove(wis.c_str());
            remove("bound/db/tables/lock.bd");
            return -1;
        }else{
            std::string conttmp=prty;
            while(true){
                size_t cn=conttmp.find(ar.str());
                if(cn==std::string::npos){
                    std::cout<<"This identification not exists"<<std::endl;
                    std::string wis="bound/db/tables/"+tableName+"/append.bd";
                    remove(wis.c_str());
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }
                std::string se=conttmp.substr(cn-3);
                se=se.substr(0,3);
                std::string fr=conttmp.substr(cn);
                size_t mrn=fr.find("<!>");
                if(se!=">|<"&&se!="<!>"){
                    conttmp=fr.substr(mrn+3);
                    continue;
                }
                if(fr.substr(0,mrn)!=argv[3]){
                    conttmp=fr.substr(mrn+3);
                    continue;
                }
                while(true){
                    int sz=0;
                    int point=0;
                    std::vector<char> seqn={'>','|','<'};
                    while(true){
                        if(seqn[point]==conttmp.substr(cn-sz)[0]){
                            point=point+1;
                            if(point==3){
                                conttmp=conttmp.substr(cn-sz);
                                break;
                            }
                        }else{
                            point=0;
                        }
                        sz=sz+1;
                    }
                    conttmp=conttmp.substr(3);
                    conttmp=conttmp.substr(0,conttmp.find("<|>"));
                    break;
                }
                std::string currensm="";
                std::string path;
                std::string ind;
                for(int spr=0;spr<3;spr++){
                    size_t xnx=conttmp.find(">|<");
                    if(spr==0){
                        size_t xnx=conttmp.find(">|<");
                        currensm=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }else if(spr==1){
                        size_t xnx=conttmp.find(">|<");
                        path=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }else{
                        size_t xnx=conttmp.find(">|<");
                        ind=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }
                }
                std::ifstream ymn("bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                std::stringstream xbuffer;
                xbuffer << ymn.rdbuf();
                std::string invr = xbuffer.str();
                ymn.close();
                std::stringstream bfr;
                bfr<<invr.substr(0,invr.find("<|>")+3);
                invr=invr.substr(invr.find("<|>")+3);
                if(stoi(ind)!=0){
                    for(int i=0;i<stoi(ind);i++){
                        bfr<<invr.substr(0,invr.find("<|>")+3);
                        invr=invr.substr(invr.find("<|>")+3);
                    }
                }
                std::string afr=invr.substr(invr.find("<|>"));
                invr=invr.substr(0,invr.find("<|>"));
                std::string tl=argv[4];
                std::vector<std::string>arr=parse_dict(tl);
                std::string rlp=invr;
                std::string nwp="";
                std::string bfrl="";
                int wn=0;
                int n_w=0;
                int qt=0;
                std::string nsn="[";
                for(int i=0;i<arr.size();i++){
                    while(true){
                        if(rlp.find(arr[i]+"{>}")!=std::string::npos){
                            if((rlp.substr(rlp.find(arr[i]+"{>}")-3).substr(0,3)==">|<"||rlp.substr(rlp.find(arr[i]+"{>}")-3).substr(0,3)=="<|>")&&(n_w==0||n_w==1)){
                                std::string prev=rlp.substr(0,rlp.find(arr[i]+"{>}"));
                                int f_c=0;
                                v_check(0,f_c,prev);
                                if(f_c!=0){
                                    std::string chn=rlp.substr(prev.size());
                                    std::string gw=v_check(1,f_c,chn);
                                    std::string r_r=prev+gw;
                                    bfrl+=rlp.substr(0,r_r.size());
                                    rlp=rlp.substr(rlp.find(r_r)+r_r.size());
                                    continue;
                                }
                                bfrl+=rlp.substr(0,rlp.find(arr[i]+"{>}")+arr[i].length()+3);
                                rlp=rlp.substr(rlp.find(arr[i]+"{>}")+arr[i].length()+3);
                                qt+=1;
                                int q_y=1;
                                rlp=v_check(2,q_y,rlp);
                                std::string tmprlp=rlp;
                                if((tmprlp.find("{>}")!=std::string::npos&&tmprlp.find("uuid_base")<tmprlp.find("{>}"))||(tmprlp.find("{>}")==std::string::npos&&tmprlp.find("uuid_base")!=std::string::npos)){
                                tmprlp=tmprlp.substr(tmprlp.find("uuid_base:^:")+12);
                                tmprlp=tmprlp.substr(0,tmprlp.find(">|<"));
                                nsn+="\""+tmprlp+"\",";
                                }
                            }else if(rlp.substr(rlp.find(arr[i]+"{>}")-3).substr(0,3)!=">|<"||rlp.substr(rlp.find(arr[i]+"{>}")-3).substr(0,3)!="<|>"){
                                std::string prev=rlp.substr(0,rlp.find(arr[i]+"{>}"));
                                int f_c=0;
                                
                                v_check(0,f_c,prev);
                                if(f_c!=0){
                                    std::string chn=rlp.substr(prev.size());
                                    std::string gw=v_check(1,f_c,chn);
                                    std::string r_r=prev+gw;
                                    bfrl+=rlp.substr(0,r_r.size());
                                    rlp=rlp.substr(rlp.find(r_r)+r_r.size());
                                    continue;
                                }
                                std::string chn=rlp.substr(prev.size());
                                std::string r_r=prev+chn.substr(0,chn.find("{<}")+3);
                                bfrl+=rlp.substr(0,r_r.size());
                                rlp=rlp.substr(rlp.find(r_r)+r_r.size());
                                continue;
                            }
                        }else{
                            std::cout<<"This param not exists: "<<arr[i]<<std::endl;
                            remove("bound/db/tables/lock.bd");
                            return -1;
                        }
                        break;
                    }
                }
                rlp=rlp.substr(0,rlp.size()-3);
                std::string am=argv[5];
                if(rlp==""){
                    if(!std::filesystem::exists("bound/db/tables/"+tableName+"/yxy__xyx")){
                        std::filesystem::create_directory("bound/db/tables/"+tableName+"/yxy__xyx");
                    }
                    std::string xsx;
                    while(true){
                        const std::string validCharacters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*";
                        std::random_device rd;
                        std::mt19937 gen(rd());
                        std::uniform_int_distribution<int> dist(0, validCharacters.size() - 1);
                        std::string generatedPassword;
                        for (int i = 0; i < 6; ++i) {
                            int charIndex = dist(gen);
                            generatedPassword += validCharacters[charIndex];
                        }
                        if (!std::filesystem::exists("bound/db/tables/"+tableName+"/yxy__xyx/"+generatedPassword+".bd")) {
                            xsx=generatedPassword;
                            break;
                        }
                    }
                    if(am!=""){
                        am+="^<>";
                    }
                    am+="{<}";
                    std::string uuid=get_uuid();
                    am="uuid_base:^:"+uuid+">|<"+am;
                    std::ofstream outputFile("bound/db/tables/"+tableName+"/yxy__xyx/"+xsx+".bd");
                    outputFile << "{>}"+am;
                    outputFile.close();
                    std::string vvv;
                    std::string end=bfrl+rlp;
                    std::string nw_uuid=get_uuid();
                    nsn+="\""+nw_uuid+"\"]";
                    vvv=bfrl+"y_n:^:y>|<uuid_base:^:"+nw_uuid+">|<pth:^:"+xsx+"^<>"+invr.substr(end.size());
                    std::ofstream xxs("bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                    xxs << bfr.str()<<vvv<<afr;
                    xxs.close();
                    remove("bound/db/tables/lock.bd");
                    std::stringstream lal;
                    lal<<"{\"n\":0,\"uuid\":\"";
                    lal<<nw_uuid;
                    lal<<"\",\"uuk\":\"";
                    lal<<uuid;
                    lal<<"\",\"uuid_b\":";
                    lal<<nsn;
                    lal<<"}";
                    std::cout<<lal.str()<<std::endl;
                    return 1;
                }else if(rlp.substr(0,3)=="y_n"&&rlp.substr(6,7)[0]=='n'){
                    std::cout<<"This child contains anothers sub-childs then it can't create contents";
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }else if(rlp.find("pth")!=std::string::npos){
                    rlp=rlp.substr(rlp.find("pth")+6);
                    rlp=rlp.substr(0,rlp.size()-3);
                    if(am!=""){
                        nsn=nsn.substr(0,nsn.size()-1);
                        nsn+="]";
                        std::ifstream aaq("bound/db/tables/"+tableName+"/yxy__xyx/"+rlp+".bd");
                        std::stringstream buffer;
                        buffer << aaq.rdbuf();
                        std::string prty = buffer.str();
                        aaq.close();

                        prty=prty.substr(0,prty.size()-3);
                        std::string uuid=get_uuid();
                        prty+="uuid_base:^:"+uuid+">|<"+am+"^<>{<}";
                        std::ofstream xxs("bound/db/tables/"+tableName+"/yxy__xyx/"+rlp+".bd");
                        xxs << prty;
                        xxs.close();
                        std::stringstream lal;
                        lal<<"{\"n\":1";
                        lal<<",\"uuk\":\"";
                        lal<<uuid;
                        lal<<"\",\"uuid_b\":";
                        lal<<nsn;
                        lal<<"}";
                        std::cout<<lal.str()<<std::endl;
                    }
                    remove("bound/db/tables/lock.bd");
                    return 1;
                }
                std::cout<<"You can't create a item in this lst"<<std::endl;
                remove("bound/db/tables/lock.bd");
                return -1;
            }
        }
    }else if(command=="findChild"){
        if(argc!=7){
            std::cout << "Usage: bound findChild <NameTable> <Identification> <name_column_array0,name_column_array1...> <specify || all> <if specify: uuid_base elif all: -1>"<<std::endl;
            return -1;
        }
        std::string tableName=argv[2];
        if(!std::filesystem::exists("bound/db/tables/"+tableName)){
            std::cout << "This table not exist" << std::endl;
            return -1;
        }
        lock();
        std::ifstream aaq("bound/db/tables/"+tableName+"/find.bd");
        std::stringstream buffer;
        buffer << aaq.rdbuf();
        std::string prty = buffer.str();
        aaq.close();
        std::stringstream ar;
        ar<<argv[3];
        ar<<"<!>";
        size_t nt=prty.find(ar.str());
        if(nt==std::string::npos){
            std::cout<<"This identification not exists"<<std::endl;
            remove("bound/db/tables/lock.bd");
            return 1;
        }else{
            std::string conttmp=prty;
            while(true){
                size_t cn=conttmp.find(ar.str());
                if(cn==std::string::npos){  
                    std::cout<<"This identification not exists"<<std::endl;
                    remove("bound/db/tables/lock.bd");
                    return 1;
                }
                std::string se=conttmp.substr(cn-3);
                se=se.substr(0,3);
                std::string fr=conttmp.substr(cn);
                size_t mrn=fr.find("<!>");
                if(se!=">|<"&&se!="<!>"){
                    conttmp=fr.substr(mrn+3);
                    continue;
                }
                if(fr.substr(0,mrn)!=argv[3]){
                    conttmp=fr.substr(mrn+3);
                    continue;
                }
                while(true){
                    int sz=0;
                    int point=0;
                    std::vector<char> seqn={'>','|','<'};
                    while(true){
                        if(seqn[point]==conttmp.substr(cn-sz)[0]){
                            point=point+1;
                            if(point==3){
                                conttmp=conttmp.substr(cn-sz);
                                break;
                            }
                        }else{
                            point=0;
                        }
                        sz=sz+1;
                    }
                    conttmp=conttmp.substr(3);
                    conttmp=conttmp.substr(0,conttmp.find("<|>"));
                    break;
                }
                std::string currensm="";
                std::string path;
                std::string ind;
                for(int spr=0;spr<3;spr++){
                    size_t xnx=conttmp.find(">|<");
                    if(spr==0){
                        size_t xnx=conttmp.find(">|<");
                        currensm=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }else if(spr==1){
                        size_t xnx=conttmp.find(">|<");
                        path=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }else{
                        size_t xnx=conttmp.find(">|<");
                        ind=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }
                }
                std::ifstream ymn("bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                std::stringstream xbuffer;
                xbuffer << ymn.rdbuf();
                std::string invr = xbuffer.str();
                ymn.close();
                std::stringstream bfr;
                bfr<<invr.substr(0,invr.find("<|>")+3);
                invr=invr.substr(invr.find("<|>")+3);
                if(stoi(ind)!=0){
                    for(int i=0;i<stoi(ind);i++){
                        bfr<<invr.substr(0,invr.find("<|>")+3);
                        invr=invr.substr(invr.find("<|>")+3);
                    }
                }
                std::stringstream xs;
                std::string afr=invr.substr(invr.find("<|>"));
                invr=invr.substr(0,invr.find("<|>"));
                std::string tl=argv[4];
                std::vector<std::string>arr=parse_dict(tl);
                std::string rlp=invr;
                std::string nwp="";
                std::string bfrl="";
                int wn=0;
                int n_w=0;
                int qt=0;
                for(int i=0;i<arr.size();i++){
                    while(true){
                        if(rlp.find(arr[i]+"{>}")!=std::string::npos){
                            if((rlp.substr(rlp.find(arr[i]+"{>}")-3).substr(0,3)==">|<"||rlp.substr(rlp.find(arr[i]+"{>}")-3).substr(0,3)=="<|>")&&(n_w==0||n_w==1)){
                                std::string prev=rlp.substr(0,rlp.find(arr[i]+"{>}"));
                                int f_c=0;
                                v_check(0,f_c,prev);
                                if(f_c!=0){
                                    std::string chn=rlp.substr(prev.size());
                                    std::string gw=v_check(1,f_c,chn);
                                    std::string r_r=prev+gw;
                                    bfrl+=rlp.substr(0,r_r.size());
                                    rlp=rlp.substr(rlp.find(r_r)+r_r.size());
                                    continue;
                                }
                                bfrl+=rlp.substr(0,rlp.find(arr[i]+"{>}")+arr[i].length()+3);
                                rlp=rlp.substr(rlp.find(arr[i]+"{>}")+arr[i].length()+3);
                                qt+=1;
                                int q_y=1;
                                rlp=v_check(2,q_y,rlp);
                            }else if(rlp.substr(rlp.find(arr[i]+"{>}")-3).substr(0,3)!=">|<"||rlp.substr(rlp.find(arr[i]+"{>}")-3).substr(0,3)!="<|>"){
                                std::string prev=rlp.substr(0,rlp.find(arr[i]+"{>}"));
                                int f_c=0;
                                
                                v_check(0,f_c,prev);
                                if(f_c!=0){
                                    std::string chn=rlp.substr(prev.size());
                                    std::string gw=v_check(1,f_c,chn);
                                    std::string r_r=prev+gw;
                                    bfrl+=rlp.substr(0,r_r.size());
                                    rlp=rlp.substr(rlp.find(r_r)+r_r.size());
                                    continue;
                                }
                                std::string chn=rlp.substr(prev.size());
                                std::string r_r=prev+chn.substr(0,chn.find("{<}")+3);
                                bfrl+=rlp.substr(0,r_r.size());
                                rlp=rlp.substr(rlp.find(r_r)+r_r.size());
                                continue;
                            }
                        }else{
                            remove("bound/db/tables/lock.bd");
                            std::cout<<"This column_array: "+arr[i]+" not exists"<<std::endl;
                            return -1;
                        }
                        break;
                    }
                }
                rlp=rlp.substr(0,rlp.size()-3);
                if(rlp==""){
                    std::cout<<"This child are empty";
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }else{
                    if(rlp.substr(0,3)=="y_n"&&rlp.substr(6,7)[0]=='n'){
                        std::cout<<"Can't edit a child w/sub-child";
                        remove("bound/db/tables/lock.bd");
                        return -1;
                    }else{
                        rlp=rlp.substr(rlp.find("pth")+6);
                        rlp=rlp.substr(0,rlp.size()-3);
                        std::ifstream aaq("bound/db/tables/"+tableName+"/yxy__xyx/"+rlp+".bd");
                        std::stringstream buffer;
                        buffer << aaq.rdbuf();
                        std::string prty = buffer.str();
                        aaq.close();
                        std::string opt=argv[5];
                        std::string _b=argv[6];
                        std::string vtx=prty;
                        if(opt=="specify"){
                            if(vtx.find("uuid_base:^:"+_b)==std::string::npos){
                                std::cout<<"this uuid_base does not exists";
                                remove("bound/db/tables/lock.bd");
                                return -1;
                            }
                            vtx=vtx.substr((vtx.find("uuid_base:^:"+_b)-3),vtx.find("uuid_base:^:"+_b)).substr(0,3);
                            if(prty.find("uuid_base:^:"+_b)==std::string::npos){
                                std::cout<<"this uuid_base does not exists";
                                remove("bound/db/tables/lock.bd");
                                return -1;
                            }
                            
                            if(vtx!="{>}"&&vtx!="^<>"){
                                std::cout<<"this uuid_base does not exists";
                                remove("bound/db/tables/lock.bd");
                                return -1;
                            }
                            std::string bfrl=prty.substr(0,prty.find("uuid_base:^:"+_b));
                            vtx=prty.substr(prty.find("uuid_base:^:"+_b));
                            std::string afrl=vtx.substr(vtx.find("^<>"));                        
                            vtx=vtx.substr(0,vtx.find("^<>"));
                            vtx=vtx.substr(0,vtx.size());
                        }
                        std::cout<<vtx<<std::endl;
                        remove("bound/db/tables/lock.bd");
                        return 1;
                    }
                }
            }
        }
    }else if(command=="erd_child"){
        if(argc!=9&&argc!=8&&argc!=10){
            std::cout << "Usage: bound erd_child <NameTable> <Identification> <name_column_array0,name_column_array1...> <uuid_base> <if edit: existent_parameter1:^:newdata1>|<new_parameter1:^:newdata2... if removeParam: existent_parameter1 if removeValue: -1... if add_param_array: nw_param:^:new_parameter> <{edit or removeParam or removeValue or add_param_array}>"<<std::endl;
            return -1;
        }
        std::string opt=argv[7];
        if(((opt=="add_param_array"||opt=="remove_param_array")&&argc!=9)||(argc!=10&&(opt=="edit_param_array"||opt=="delete_param_array"))||(argc!=8&&(opt=="edit"||opt=="removeParam"||opt=="removeValue"))){
            std::cout << "Usage: bound erd_child <NameTable> <Identification> <name_column_array0,name_column_array1...> <uuid_base> <if edit: existent_parameter1:^:newdata1>|<new_parameter1:^:newdata2... if removeParam: existent_parameter1 if removeValue: -1... if add_param_array: nw_param:^:new_parameter> <{edit or removeParam or removeValue or add_param_array}>"<<std::endl;
            return -1;
        }
        std::string tableName=argv[2];
        if(!std::filesystem::exists("bound/db/tables/"+tableName)){
            std::cout << "This table not exist" << std::endl;
            return -1;
        }
        lock();
        std::ifstream aaq("bound/db/tables/"+tableName+"/find.bd");
        std::stringstream buffer;
        buffer << aaq.rdbuf();
        std::string prty = buffer.str();
        aaq.close();
        std::stringstream ar;
        ar<<argv[3];
        ar<<"<!>";
        size_t nt=prty.find(ar.str());
        if(nt==std::string::npos){
            std::cout<<"This identification not exists"<<std::endl;
            remove("bound/db/tables/lock.bd");
            return 1;
        }else{
            std::string conttmp=prty;
            while(true){
                size_t cn=conttmp.find(ar.str());
                if(cn==std::string::npos){  
                    std::cout<<"This identification not exists"<<std::endl;
                    remove("bound/db/tables/lock.bd");
                    return 1;
                }
                std::string se=conttmp.substr(cn-3);
                se=se.substr(0,3);
                std::string fr=conttmp.substr(cn);
                size_t mrn=fr.find("<!>");
                if(se!=">|<"&&se!="<!>"){
                    conttmp=fr.substr(mrn+3);
                    continue;
                }
                if(fr.substr(0,mrn)!=argv[3]){
                    conttmp=fr.substr(mrn+3);
                    continue;
                }
                while(true){
                    int sz=0;
                    int point=0;
                    std::vector<char> seqn={'>','|','<'};
                    while(true){
                        if(seqn[point]==conttmp.substr(cn-sz)[0]){
                            point=point+1;
                            if(point==3){
                                conttmp=conttmp.substr(cn-sz);
                                break;
                            }
                        }else{
                            point=0;
                        }
                        sz=sz+1;
                    }
                    conttmp=conttmp.substr(3);
                    conttmp=conttmp.substr(0,conttmp.find("<|>"));
                    break;
                }
                std::string currensm="";
                std::string path;
                std::string ind;
                for(int spr=0;spr<3;spr++){
                    size_t xnx=conttmp.find(">|<");
                    if(spr==0){
                        size_t xnx=conttmp.find(">|<");
                        currensm=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }else if(spr==1){
                        size_t xnx=conttmp.find(">|<");
                        path=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }else{
                        size_t xnx=conttmp.find(">|<");
                        ind=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }
                }
                std::ifstream ymn("bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                std::stringstream xbuffer;
                xbuffer << ymn.rdbuf();
                std::string invr = xbuffer.str();
                ymn.close();
                std::stringstream bfr;
                bfr<<invr.substr(0,invr.find("<|>")+3);
                invr=invr.substr(invr.find("<|>")+3);
                if(stoi(ind)!=0){
                    for(int i=0;i<stoi(ind);i++){
                        bfr<<invr.substr(0,invr.find("<|>")+3);
                        invr=invr.substr(invr.find("<|>")+3);
                    }
                }
                std::stringstream xs;
                std::string afr=invr.substr(invr.find("<|>"));
                invr=invr.substr(0,invr.find("<|>"));
                std::string tl=argv[4];
                std::vector<std::string>arr=parse_dict(tl);
                std::string rlp=invr;
                std::string nwp="";
                std::string bfrl="";
                int wn=0;
                int n_w=0;
                int qt=0;
                for(int i=0;i<arr.size();i++){
                    while(true){
                        if(rlp.find(arr[i]+"{>}")!=std::string::npos){
                            if((rlp.substr(rlp.find(arr[i]+"{>}")-3).substr(0,3)==">|<"||rlp.substr(rlp.find(arr[i]+"{>}")-3).substr(0,3)=="<|>")&&(n_w==0||n_w==1)){
                                std::string prev=rlp.substr(0,rlp.find(arr[i]+"{>}"));
                                int f_c=0;
                                v_check(0,f_c,prev);
                                if(f_c!=0){
                                    std::string chn=rlp.substr(prev.size());
                                    std::string gw=v_check(1,f_c,chn);
                                    std::string r_r=prev+gw;
                                    bfrl+=rlp.substr(0,r_r.size());
                                    rlp=rlp.substr(rlp.find(r_r)+r_r.size());
                                    continue;
                                }
                                bfrl+=rlp.substr(0,rlp.find(arr[i]+"{>}")+arr[i].length()+3);
                                rlp=rlp.substr(rlp.find(arr[i]+"{>}")+arr[i].length()+3);
                                qt+=1;
                                int q_y=1;
                                rlp=v_check(2,q_y,rlp);
                            }else if(rlp.substr(rlp.find(arr[i]+"{>}")-3).substr(0,3)!=">|<"||rlp.substr(rlp.find(arr[i]+"{>}")-3).substr(0,3)!="<|>"){
                                std::string prev=rlp.substr(0,rlp.find(arr[i]+"{>}"));
                                int f_c=0;
                                
                                v_check(0,f_c,prev);
                                if(f_c!=0){
                                    std::string chn=rlp.substr(prev.size());
                                    std::string gw=v_check(1,f_c,chn);
                                    std::string r_r=prev+gw;
                                    bfrl+=rlp.substr(0,r_r.size());
                                    rlp=rlp.substr(rlp.find(r_r)+r_r.size());
                                    continue;
                                }
                                std::string chn=rlp.substr(prev.size());
                                std::string r_r=prev+chn.substr(0,chn.find("{<}")+3);
                                bfrl+=rlp.substr(0,r_r.size());
                                rlp=rlp.substr(rlp.find(r_r)+r_r.size());
                                continue;
                            }
                        }
                        else{
                            remove("bound/db/tables/lock.bd");
                            std::cout<<"This column_array: "+arr[i]+" not exists"<<std::endl;
                            return -1;
                        }
                        break;
                    }
                }
                rlp=rlp.substr(0,rlp.size()-3);
                if(rlp==""){
                    std::cout<<"This child are empty";
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }else{
                    if(rlp.substr(0,3)=="y_n"&&rlp.substr(6,7)[0]=='n'){
                        std::cout<<"Can't edit a child w/sub-child";
                        remove("bound/db/tables/lock.bd");
                        return -1;
                    }else{
                        rlp=rlp.substr(rlp.find("pth")+6);
                        rlp=rlp.substr(0,rlp.size()-3);
                        std::ifstream aaq("bound/db/tables/"+tableName+"/yxy__xyx/"+rlp+".bd");
                        std::stringstream buffer;
                        buffer << aaq.rdbuf();
                        std::string prty = buffer.str();
                        aaq.close();
                        std::string _b=argv[5];
                        if(prty.find("uuid_base:^:"+_b)==std::string::npos){
                            std::cout<<"this uuid_base does not exists";
                            remove("bound/db/tables/lock.bd");
                            return -1;
                        }
                            
                        std::string vtx=prty.substr((prty.find("uuid_base:^:"+_b)-3),prty.find("uuid_base:^:"+_b)).substr(0,3);
                        if(vtx!="{>}"&&vtx!="^<>"){
                            std::cout<<"this uuid_base does not exists";
                            remove("bound/db/tables/lock.bd");
                            return -1;
                        }
                        std::string bfrl=prty.substr(0,prty.find("uuid_base:^:"+_b));
                        vtx=prty.substr(prty.find("uuid_base:^:"+_b));
                        std::string afrl=vtx.substr(vtx.find("^<>"));                        
                        vtx=vtx.substr(0,vtx.find("^<>"));
                        vtx=vtx.substr(0,vtx.size());
                        std::string vp=argv[6];
                        if(opt=="edit"){
                            std::vector<std::string> dict=parseKV(vp);
                            for (const auto& arg : dict) {
                                std::vector<std::string>params=parseP(arg);
                                if(params[0]=="uuid_base"){
                                    continue;
                                }
                                if(vtx.find(params[0]+":^:")==std::string::npos){
                                    vtx=vtx+">|<"+params[0]+":^:"+params[1];
                                }else{
                                    if(vtx.substr(vtx.find(params[0]+":^:")-3).substr(0,3)!=">|<"){
                                        std::string bfp=vtx.substr(0,vtx.find(params[0]+":^:")+(params[0]+":^:").size());
                                        std::string t_vtx=vtx.substr(vtx.find(params[0]+":^:")+(params[0]+":^:").size());
                                        bfp+=t_vtx.substr(0,t_vtx.find(">|<"));
                                        t_vtx=t_vtx.substr(t_vtx.find(">|<"));
                                        int stt=-1;
                                        while(true){
                                            if(t_vtx.find(params[0]+":^:")==std::string::npos){
                                                stt=0;
                                                break;
                                            }else if(t_vtx.substr(t_vtx.find(params[0]+":^:")-3).substr(0,3)!=">|<"){
                                                bfp+=t_vtx.substr(0,t_vtx.find(params[0]+":^:")+(params[0]+":^:").size());
                                                t_vtx=t_vtx.substr(t_vtx.find(params[0]+":^:")+(params[0]+":^:").size());
                                                bfp+=t_vtx.substr(0,t_vtx.find(">|<"));
                                                t_vtx=t_vtx.substr(t_vtx.find(">|<"));
                                            }else{
                                                stt=1;
                                                break;
                                            }
                                        }
                                        if(stt=0){
                                            t_vtx="";
                                            bfp="";
                                            vtx=vtx+">|<"+params[0]+":^:"+params[1];
                                        }else{
                                            t_vtx=t_vtx.substr(3,t_vtx.size());
                                            if(t_vtx.find(">|<")!=std::string::npos){
                                                t_vtx=t_vtx.substr(0,t_vtx.find(">|<"));
                                            }
                                            afr=vtx.substr((bfp+">|<"+t_vtx).size());
                                            vtx=bfp+">|<"+params[0]+":^:"+params[1]+afr;
                                        }
                                    }else{
                                        std::string bfp=vtx.substr(0,vtx.find(params[0]+":^:"));
                                        std::string t_vtx=vtx.substr(vtx.find(params[0]+":^:"));
                                        std::string afr="";
                                        if(t_vtx.find(">|<")!=std::string::npos){
                                            std::string end=t_vtx.substr(0,t_vtx.find(">|<"));
                                            afr=vtx.substr((bfp+end).size());
                                        }
                                        vtx=bfp+params[0]+":^:"+params[1]+afr;
                                    }
                                }
                            }
                            std::ofstream outputFile("bound/db/tables/"+tableName+"/yxy__xyx/"+rlp+".bd");
                            outputFile<<bfrl<<vtx<<afrl;
                            outputFile.close();
                        }else if(opt=="removeParam"){
                            std::string b_r="";
                            while(true){
                                if(vp=="uuid_base"){
                                    std::cout<<"This param is not allowed"<<std::endl;
                                    break;
                                }
                                if(vtx.find(vp+":^:")==std::string::npos){
                                    std::cout<<"This param not exists"<<std::endl;
                                    break;
                                }else{
                                    std::string chk=vtx.substr(vtx.find(vp)-3).substr(0,3);
                                    if(chk==">|<"){
                                        std::string zcbf=vtx.substr(0,vtx.find(vp+":^:")-3);
                                        vtx=vtx.substr(vtx.find(vp+":^:"));
                                        if(vtx.find(">|<")!=std::string::npos){
                                            vtx=vtx.substr(vtx.find(">|<"));
                                        }else{
                                            vtx="";
                                        }
                                        std::ofstream outputFile("bound/db/tables/"+tableName+"/yxy__xyx/"+rlp+".bd");
                                        outputFile<<bfrl<<b_r<<zcbf<<vtx<<afrl;
                                        outputFile.close();
                                        break;
                                    }
                                    if(vtx.find(">|<")==std::string::npos||vtx.find(vp+":^:")==std::string::npos){
                                        std::cout<<"This param not exists"<<std::endl;
                                        break;
                                    }
                                    b_r=vtx.substr(0,vtx.find(vp+":^:"));
                                    vtx=vtx.substr(vtx.find(vp+":^:"));
                                    b_r+=vtx.substr(0,vtx.find(">|<"));
                                    vtx=vtx.substr(vtx.find(">|<"));
                                }
                            }
                            
                        }else if(opt=="removeValue"&&vp=="-1"){
                            std::string bfr=prty.substr(0,prty.find(vtx));
                            std::string b=prty.substr(prty.find(vtx)+vtx.size()+3);
                            std::ofstream outputFile("bound/db/tables/"+tableName+"/yxy__xyx/"+rlp+".bd");
                            outputFile<<bfr<<b;
                            outputFile.close();
                        }else if(opt=="add_param_array"){
                            std::string uuid=get_uuid();
                            std::string pxlo=argv[6];
                            std::string nvvc=argv[8];
                            size_t rs=nvvc.find(":^:");
                            if(rs==std::string::npos){
                                std::cout<<"Your parameters can't be empty"<<std::endl;
                                remove("bound/db/tables/lock.bd");
                                return -1;
                            }
                            if(vtx.find(pxlo)==std::string::npos){
                                std::stringstream nxw;
                                nxw<<vtx<<">|<"<<pxlo<<"!@!base:^:0>|<uuid_base:^:"<<uuid<<">|<"<<nvvc<<"^><!#!";
                                std::ofstream outputFile("bound/db/tables/"+tableName+"/yxy__xyx/"+rlp+".bd");
                                outputFile<<bfrl<<nxw.str()<<afrl;
                                outputFile.close();
                                std::cout<<uuid<<std::endl;
                                remove("bound/db/tables/lock.bd");
                                return -1;
                            }
                            size_t arr=vtx.find(pxlo);
                            std::string tmpchild=vtx.substr(arr);
                            std::stringstream nxcheck;
                            nxcheck<<pxlo<<"!@!";
                            if(tmpchild.find(nxcheck.str())==std::string::npos){
                                std::stringstream nxw;
                                nxw<<pxlo<<"!@!base:^:0>|<uuid_base:^:"<<uuid<<">|<"<<nvvc<<"^><!#!";
                                std::stringstream nwimp;
                                nwimp<<vtx<<">|<"<<nxw.str();
                                std::string out=bfrl+nwimp.str()+afrl;
                                std::ofstream outputFile("bound/db/tables/"+tableName+"/yxy__xyx/"+rlp+".bd");
                                outputFile << out;
                                outputFile.close();
                                std::cout<<uuid<<std::endl;
                                remove("bound/db/tables/lock.bd");
                                return -1;
                            }
                            tmpchild=tmpchild.substr(0,tmpchild.find("!#!"));
                            std::string tmploop=tmpchild;
                            int baseN=0;
                            if(tmpchild.find("^><")!=std::string::npos){
                                while(true){
                                    tmploop=tmploop.substr(tmploop.find("^><")+3);
                                    baseN=baseN+1;
                                    if(tmploop==""){
                                        break;
                                    }
                                }
                            }
                            std::stringstream xnw;
                            xnw<<"base:^:"<<std::to_string(baseN)<<">|<uuid_base:^:"<<uuid<<">|<"<<nvvc<<"^><";
                            std::string bfrinvr=vtx.substr(0,vtx.find(tmpchild));
                            std::string afrinvr=vtx.substr(vtx.find(tmpchild)+tmpchild.length());
                            std::ofstream outputFile("bound/db/tables/"+tableName+"/yxy__xyx/"+rlp+".bd");
                            outputFile << bfrl<<bfrinvr<<tmpchild<<xnw.str()<<afrinvr<<afrl;
                            outputFile.close();
                            remove("bound/db/tables/lock.bd");
                            std::cout<<uuid<<std::endl;
                            return 0;
                        }else if(opt=="edit_param_array"){
                            std::string uuid=get_uuid();
                            std::string pxlo=argv[6];
                            std::string nvvc=argv[8];
                            std::string cnst=argv[9];
                            if(vtx.find(pxlo)==std::string::npos){
                                std::cout<<"This column_array is not find"; 
                                remove("bound/db/tables/lock.bd");
                                return -1;
                            }
                            size_t arr=vtx.find(pxlo);
                            std::string tmpchild=vtx.substr(arr);
                            std::cout<<tmpchild<<std::endl;
                            std::cout<<"sl"<<std::endl;
                            std::cout<<vtx<<std::endl;
                            std::stringstream nxcheck;
                            nxcheck<<pxlo<<"!@!";
                            if(tmpchild.find(nxcheck.str())==std::string::npos){
                                std::cout<<"This column_array is not find"; 
                                remove("bound/db/tables/lock.bd");
                                return -1;
                            }
                            std::string tmploop=tmpchild;                            
                            size_t rs=nvvc.find(":^:");
                            if(rs==std::string::npos){
                                std::cout<<"Your parameters can't be empty"<<std::endl;
                                remove("bound/db/tables/lock.bd");
                                return -1;
                            }
                            remove("bound/db/tables/lock.bd");
                            return 1;
                        }
                        remove("bound/db/tables/lock.bd");
                        return 1;
                    }
                }
            }
        }
    }else if(command=="remove_param_colChild"){
        if(argc!=7){
            std::cout << "Usage: bound edit_param_child <NameTable> <Identification> <name_column_array0,name_column_array1...> <uuid_base> <existent_parameter1:^:newdata1>|<new_parameter1:^:newdata2...>"<<std::endl;
            return -1;
        }
        std::string tableName=argv[2];
        if(!std::filesystem::exists("bound/db/tables/"+tableName)){
            std::cout << "This table not exist" << std::endl;
            return -1;
        }
        lock();
        std::ifstream aaq("bound/db/tables/"+tableName+"/find.bd");
        std::stringstream buffer;
        buffer << aaq.rdbuf();
        std::string prty = buffer.str();
        aaq.close();
        std::stringstream ar;
        ar<<argv[3];
        ar<<"<!>";
        size_t nt=prty.find(ar.str());
        if(nt==std::string::npos){
            std::cout<<"This identification not exists"<<std::endl;
            remove("bound/db/tables/lock.bd");
            return 1;
        }else{
            std::string conttmp=prty;
            while(true){
                size_t cn=conttmp.find(ar.str());
                if(cn==std::string::npos){  
                    std::cout<<"This identification not exists"<<std::endl;
                    remove("bound/db/tables/lock.bd");
                    return 1;
                }
                std::string se=conttmp.substr(cn-3);
                se=se.substr(0,3);
                std::string fr=conttmp.substr(cn);
                size_t mrn=fr.find("<!>");
                if(se!=">|<"&&se!="<!>"){
                    conttmp=fr.substr(mrn+3);
                    continue;
                }
                if(fr.substr(0,mrn)!=argv[3]){
                    conttmp=fr.substr(mrn+3);
                    continue;
                }
                while(true){
                    int sz=0;
                    int point=0;
                    std::vector<char> seqn={'>','|','<'};
                    while(true){
                        if(seqn[point]==conttmp.substr(cn-sz)[0]){
                            point=point+1;
                            if(point==3){
                                conttmp=conttmp.substr(cn-sz);
                                break;
                            }
                        }else{
                            point=0;
                        }
                        sz=sz+1;
                    }
                    conttmp=conttmp.substr(3);
                    conttmp=conttmp.substr(0,conttmp.find("<|>"));
                    break;
                }
                std::string currensm="";
                std::string path;
                std::string ind;
                for(int spr=0;spr<3;spr++){
                    size_t xnx=conttmp.find(">|<");
                    if(spr==0){
                        size_t xnx=conttmp.find(">|<");
                        currensm=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }else if(spr==1){
                        size_t xnx=conttmp.find(">|<");
                        path=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }else{
                        size_t xnx=conttmp.find(">|<");
                        ind=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }
                }
                std::ifstream ymn("bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                std::stringstream xbuffer;
                xbuffer << ymn.rdbuf();
                std::string invr = xbuffer.str();
                ymn.close();
                std::stringstream bfr;
                bfr<<invr.substr(0,invr.find("<|>")+3);
                invr=invr.substr(invr.find("<|>")+3);
                if(stoi(ind)!=0){
                    for(int i=0;i<stoi(ind);i++){
                        bfr<<invr.substr(0,invr.find("<|>")+3);
                        invr=invr.substr(invr.find("<|>")+3);
                    }
                }
                std::stringstream xs;
                std::string afr=invr.substr(invr.find("<|>"));
                invr=invr.substr(0,invr.find("<|>"));
                std::string tl=argv[4];
                std::vector<std::string>arr=parse_dict(tl);
                std::string rlp=invr;
                std::string nwp="";
                std::string bfrl="";
                int wn=0;
                int n_w=0;
                int qt=0;
                for(int i=0;i<arr.size();i++){
                    while(true){
                        if(rlp.find(arr[i]+"{>}")!=std::string::npos){
                            if((rlp.substr(rlp.find(arr[i]+"{>}")-3).substr(0,3)==">|<"||rlp.substr(rlp.find(arr[i]+"{>}")-3).substr(0,3)=="<|>")&&(n_w==0||n_w==1)){
                                std::string prev=rlp.substr(0,rlp.find(arr[i]+"{>}"));
                                int f_c=0;
                                v_check(0,f_c,prev);
                                if(f_c!=0){
                                    std::string chn=rlp.substr(prev.size());
                                    std::string gw=v_check(1,f_c,chn);
                                    std::string r_r=prev+gw;
                                    bfrl+=rlp.substr(0,r_r.size());
                                    rlp=rlp.substr(rlp.find(r_r)+r_r.size());
                                    continue;
                                }
                                bfrl+=rlp.substr(0,rlp.find(arr[i]+"{>}")+arr[i].length()+3);
                                rlp=rlp.substr(rlp.find(arr[i]+"{>}")+arr[i].length()+3);
                                qt+=1;
                                int q_y=1;
                                rlp=v_check(2,q_y,rlp);
                            }else if(rlp.substr(rlp.find(arr[i]+"{>}")-3).substr(0,3)!=">|<"||rlp.substr(rlp.find(arr[i]+"{>}")-3).substr(0,3)!="<|>"){
                                std::string prev=rlp.substr(0,rlp.find(arr[i]+"{>}"));
                                int f_c=0;
                                
                                v_check(0,f_c,prev);
                                if(f_c!=0){
                                    std::string chn=rlp.substr(prev.size());
                                    std::string gw=v_check(1,f_c,chn);
                                    std::string r_r=prev+gw;
                                    bfrl+=rlp.substr(0,r_r.size());
                                    rlp=rlp.substr(rlp.find(r_r)+r_r.size());
                                    continue;
                                }
                                std::string chn=rlp.substr(prev.size());
                                std::string r_r=prev+chn.substr(0,chn.find("{<}")+3);
                                bfrl+=rlp.substr(0,r_r.size());
                                rlp=rlp.substr(rlp.find(r_r)+r_r.size());
                                continue;
                            }
                        }else{
                            remove("bound/db/tables/lock.bd");
                            std::cout<<"This column_array: "+arr[i]+" not exists"<<std::endl;
                            return -1;
                        }
                        break;
                    }
                }
                rlp=rlp.substr(0,rlp.size()-3);
                if(rlp==""){
                    std::cout<<"This child are empty";
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }else{
                    if(rlp.substr(0,3)=="y_n"&&rlp.substr(6,7)[0]=='n'){
                        std::cout<<"Can't edit a child w/sub-child";
                        remove("bound/db/tables/lock.bd");
                        return -1;
                    }else{
                        rlp=rlp.substr(rlp.find("pth")+6);
                        rlp=rlp.substr(0,rlp.size()-3);
                        std::ifstream aaq("bound/db/tables/"+tableName+"/yxy__xyx/"+rlp+".bd");
                        std::stringstream buffer;
                        buffer << aaq.rdbuf();
                        std::string prty = buffer.str();
                        aaq.close();
                        std::string _b=argv[5];
                        if(prty.find("uuid_base:^:"+_b)==std::string::npos){
                            std::cout<<"this uuid_base does not exists";
                            remove("bound/db/tables/lock.bd");
                            return -1;
                        }
                        std::string vtx=prty.substr((prty.find("uuid_base:^:"+_b)-3),prty.find("uuid_base:^:"+_b)).substr(0,3);
                        if(vtx!="{<}"&&vtx!="^<>"){
                            std::cout<<"this uuid_base does not exists";
                            remove("bound/db/tables/lock.bd");
                            return -1;
                        }
                        std::string bfrl=prty.substr(0,prty.find("uuid_base:^:"+_b));
                        vtx=prty.substr(prty.find("uuid_base:^:"+_b));
                        std::string afrl=vtx.substr(vtx.find("^<>"));                        
                        vtx=vtx.substr(0,vtx.find("^<>"));
                        vtx=vtx.substr(0,vtx.size());
                        std::cout<<vtx<<std::endl;
                        remove("bound/db/tables/lock.bd");
                        return -1;
                    }
                }
            }
        }
    }else if(command=="find_param_array"){
        if(argc!=6){
            std::cout << "Usage: bound find_param_array <NameTable> <Identification> <name_column_array> <uuid_base>" << std::endl;
            return -1;
        }
        std::string tableName=argv[2];
        if(!std::filesystem::exists("bound/db/tables/"+tableName)){
            std::cout << "This table not exist" << std::endl;
            return -1;
        }
        lock();
        std::ifstream aaq("bound/db/tables/"+tableName+"/find.bd");
        std::stringstream buffer;
        buffer << aaq.rdbuf();
        std::string prty = buffer.str();
        aaq.close();
        std::stringstream ar;
        ar<<argv[3];
        ar<<"<!>";
        size_t nt=prty.find(ar.str());
        if(nt==std::string::npos){
            std::cout<<"This identification not exists"<<std::endl;
            remove("bound/db/tables/lock.bd");
            return 1;
        }else{
            std::string conttmp=prty;
            while(true){
                size_t cn=conttmp.find(ar.str());
                if(cn==std::string::npos){
                    std::cout<<"This identification not exists"<<std::endl;
                    remove("bound/db/tables/lock.bd");
                    return 1;
                }
                std::string se=conttmp.substr(cn-3);
                se=se.substr(0,3);
                std::string fr=conttmp.substr(cn);
                size_t mrn=fr.find("<!>");
                if(se!=">|<"&&se!="<!>"){
                    conttmp=fr.substr(mrn+3);
                    continue;
                }
                if(fr.substr(0,mrn)!=argv[3]){
                    conttmp=fr.substr(mrn+3);
                    continue;
                }
                while(true){
                    int sz=0;
                    int point=0;
                    std::vector<char> seqn={'>','|','<'};
                    while(true){
                        if(seqn[point]==conttmp.substr(cn-sz)[0]){
                            point=point+1;
                            if(point==3){
                                conttmp=conttmp.substr(cn-sz);
                                break;
                            }
                        }else{
                            point=0;
                        }
                        sz=sz+1;
                    }
                    conttmp=conttmp.substr(3);
                    conttmp=conttmp.substr(0,conttmp.find("<|>"));
                    break;
                }
                std::string currensm="";
                std::string path;
                std::string ind;
                for(int spr=0;spr<3;spr++){
                    size_t xnx=conttmp.find(">|<");
                    if(spr==0){
                        size_t xnx=conttmp.find(">|<");
                        currensm=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }else if(spr==1){
                        size_t xnx=conttmp.find(">|<");
                        path=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }else{
                        size_t xnx=conttmp.find(">|<");
                        ind=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }
                }
                std::ifstream ymn("bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                std::stringstream xbuffer;
                xbuffer << ymn.rdbuf();
                std::string invr = xbuffer.str();
                ymn.close();
                std::stringstream bfr;
                bfr<<invr.substr(0,invr.find("<|>")+3);
                invr=invr.substr(invr.find("<|>")+3);
                if(stoi(ind)!=0){
                    for(int i=0;i<stoi(ind);i++){
                        bfr<<invr.substr(0,invr.find("<|>")+3);
                        invr=invr.substr(invr.find("<|>")+3);
                    }
                }
                    
                std::stringstream xs;
                std::string afr=invr.substr(invr.find("<|>"));
                invr=invr.substr(0,invr.find("<|>"));            
                std::stringstream cxc;
                cxc<<argv[4]<<"{>}";
                if(invr.find(cxc.str())==std::string::npos){
                    std::cout<<"This name_column_array not exists";
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }else{
                    std::string tmpinvr=invr;
                    while(true){
                        if(tmpinvr.find(cxc.str())==std::string::npos){
                            std::cout<<"This name_column_array not exists"<<std::endl;
                            remove("bound/db/tables/lock.bd");
                            return -1;
                        }
                        std::string sxe=tmpinvr.substr(tmpinvr.find(cxc.str()));
                        std::string se=tmpinvr.substr(tmpinvr.find(cxc.str())-3);
                        if(se.substr(0,3)!=">|<"&&se.substr(0,3)!="<|>"){
                            tmpinvr=tmpinvr.substr(tmpinvr.find(cxc.str())+cxc.str().length());
                            continue;
                        }
                        sxe=sxe.substr(0,sxe.find("{<}")+3);
                        std::string sxetmp=sxe;
                        std::stringstream bs;
                        bs<<"uuid_base:^:"<<argv[5];
                        while(true){
                            if(sxetmp.find(bs.str())==std::string::npos){
                                std::cout<<"This base_identification not exists"<<std::endl;
                                remove("bound/db/tables/lock.bd");
                                return -1;
                            }else{
                                std::string seb=sxetmp.substr(sxetmp.find(bs.str())-3);
                                sxetmp=sxetmp.substr(sxetmp.find(bs.str()));
                                seb=seb.substr(0,3);
                                if(seb!=">|<"){
                                    sxetmp=sxetmp.substr(sxetmp.find(bs.str())+bs.str().length());
                                    continue;
                                }
                                if(sxetmp.substr(bs.str().length()).substr(0,3)!=">|<"&&sxetmp.substr(bs.str().length()).substr(0,3)!="^<>"){
                                    sxetmp=sxetmp.substr(sxetmp.find(bs.str())+bs.str().length());
                                    continue;                                    
                                }
                                std::string nxnew=sxetmp.substr(0,sxetmp.find("^<>"));
                                std::vector<char>v_5={':','^',':'};
                                int p=0;
                                int point=0;
                                while(true){
                                    p=p+1;
                                    if(sxe.substr(sxe.find(nxnew)-p)[0]==v_5[point]){
                                        point=point+1;
                                    }else{
                                        point=0;
                                    }
                                    if(point==3){
                                        sxe=sxe.substr(sxe.find(nxnew)-p);
                                        sxe=sxe.substr(3,sxe.find("^<>")-3);
                                        std::cout<<"base:^:"<<sxe<<std::endl;
                                        remove("bound/db/tables/lock.bd");
                                        return -1;
                                    }
                                }
                                remove("bound/db/tables/lock.bd");
                                return -1;
                            }
                        }
                    }
                }
            }
        }
    }else if(command=="edit_paramI_array"){
        if(argc!=7){
            std::cout << "Usage: bound edit_param_array <NameTable> <Identification> <name_column_array> <uuid_base> <user:^:DarkSouls_23>|<comment:^:Hello>" << std::endl;
            return -1;
        }
        std::string tableName=argv[2];
        if(!std::filesystem::exists("bound/db/tables/"+tableName)){
            std::cout << "This table not exist" << std::endl;
            return -1;
        }
        lock();
        std::ifstream aaq("bound/db/tables/"+tableName+"/find.bd");
        std::stringstream buffer;
        buffer << aaq.rdbuf();
        std::string prty = buffer.str();
        aaq.close();
        std::stringstream ar;
        ar<<argv[3];
        ar<<"<!>";
        size_t nt=prty.find(ar.str());
        if(nt==std::string::npos){
            std::cout<<"This identification not exists"<<std::endl;
            remove("bound/db/tables/lock.bd");
            return 1;
        }else{
            std::string conttmp=prty;
            while(true){
                size_t cn=conttmp.find(ar.str());
                if(cn==std::string::npos){
                    std::cout<<"This identification not exists"<<std::endl;
                    remove("bound/db/tables/lock.bd");
                    return 1;
                }
                std::string se=conttmp.substr(cn-3);
                se=se.substr(0,3);
                std::string fr=conttmp.substr(cn);
                size_t mrn=fr.find("<!>");
                if(se!=">|<"&&se!="<!>"){
                    conttmp=fr.substr(mrn+3);
                    continue;
                }
                if(fr.substr(0,mrn)!=argv[3]){
                    conttmp=fr.substr(mrn+3);
                    continue;
                }
                while(true){
                    int sz=0;
                    int point=0;
                    std::vector<char> seqn={'>','|','<'};
                    while(true){
                        if(seqn[point]==conttmp.substr(cn-sz)[0]){
                            point=point+1;
                            if(point==3){
                                conttmp=conttmp.substr(cn-sz);
                                break;
                            }
                        }else{
                            point=0;
                        }
                        sz=sz+1;
                    }
                    conttmp=conttmp.substr(3);
                    conttmp=conttmp.substr(0,conttmp.find("<|>"));
                    break;
                }
                std::string currensm="";
                std::string path;
                std::string ind;
                for(int spr=0;spr<3;spr++){
                    size_t xnx=conttmp.find(">|<");
                    if(spr==0){
                        size_t xnx=conttmp.find(">|<");
                        currensm=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }else if(spr==1){
                        size_t xnx=conttmp.find(">|<");
                        path=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }else{
                        size_t xnx=conttmp.find(">|<");
                        ind=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }
                }
                std::ifstream ymn("bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                std::stringstream xbuffer;
                xbuffer << ymn.rdbuf();
                std::string invr = xbuffer.str();
                ymn.close();
                std::stringstream bfr;
                bfr<<invr.substr(0,invr.find("<|>")+3);
                invr=invr.substr(invr.find("<|>")+3);
                if(stoi(ind)!=0){
                    for(int i=0;i<stoi(ind);i++){
                        bfr<<invr.substr(0,invr.find("<|>")+3);
                        invr=invr.substr(invr.find("<|>")+3);
                    }
                }
                    
                std::stringstream xs;
                std::string afr=invr.substr(invr.find("<|>"));
                invr=invr.substr(0,invr.find("<|>"));            
                std::stringstream cxc;
                cxc<<argv[4]<<"!@!";
                if(invr.find(cxc.str())==std::string::npos){
                    std::cout<<"This name_column_array not exists";
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }else{
                    std::string tmpinvr=invr;
                    std::vector<std::string>glt={"","",""};
                    while(true){
                        if(tmpinvr.find(cxc.str())==std::string::npos){
                            std::cout<<"This name_column_array not exists"<<std::endl;
                            remove("bound/db/tables/lock.bd");
                            return -1;
                        }
                        std::string sxe=tmpinvr.substr(tmpinvr.find(cxc.str()));
                        std::string se=tmpinvr.substr(tmpinvr.find(cxc.str())-3);
                        if(se.substr(0,3)!=">|<"&&se.substr(0,3)!="<|>"){
                            tmpinvr=tmpinvr.substr(tmpinvr.find(cxc.str())+cxc.str().length());
                            continue;
                        }
                        sxe=sxe.substr(0,sxe.find("!#!")+3);
                        std::string sxetmp=sxe;
                        std::stringstream bs;
                        bs<<"uuid_base:^:"<<argv[5];
                        while(true){
                            if(sxetmp.find(bs.str())==std::string::npos){
                                std::cout<<"This base_identification not exists"<<std::endl;
                                remove("bound/db/tables/lock.bd");
                                return -1;
                            }else{
                                std::string seb=sxetmp.substr(sxetmp.find(bs.str())-3);
                                sxetmp=sxetmp.substr(sxetmp.find(bs.str()));
                                seb=seb.substr(0,3);
                                if(seb!=">|<"){
                                    sxetmp=sxetmp.substr(sxetmp.find(bs.str())+bs.str().length());
                                    continue;
                                }
                                if(sxetmp.substr(bs.str().length()).substr(0,3)!=">|<"&&sxetmp.substr(bs.str().length()).substr(0,3)!="^<>"){
                                    sxetmp=sxetmp.substr(sxetmp.find(bs.str())+bs.str().length());
                                    continue;                                    
                                }
                                glt[1]=sxe.substr(0,sxe.find(sxetmp));
                                std::string prms=argv[6];
                                std::vector<char>v_0={':','^',':'};
                                int p_0=0;
                                std::vector<char>v_1={'>','|','<'};
                                int p_1=0;
                                std::vector<std::string>p_s={"",""};
                                int step=0;
                                std::string nxnew=sxetmp.substr(0,sxetmp.find("^<>"));
                                for(int i=0;i<prms.length();i++){
                                    if(step==0){
                                        p_s[0]+=prms[i];
                                        if(prms[i]==v_0[p_0]){
                                            p_0+=1;
                                            if(p_0==3){
                                                p_s[0]=p_s[0].substr(0,p_s[0].find(":^:"));
                                                step=1;
                                            }
                                        }
                                    }else if(step==1){
                                        p_s[1]+=prms[i];
                                        if(prms[i]==v_1[p_1]){
                                            p_1+=1;
                                            if(p_1==3){
                                                p_s[1]=p_s[1].substr(0,p_s[1].find(">|<"));
                                                step=2;
                                            }
                                        }
                                    }else if(step==2){
                                        if(p_s[0]!="base"){
                                            bool exist=false;
                                            if(nxnew.find(p_s[0])!=std::string::npos){
                                                std::string nxnewtmp=nxnew;
                                                
                                                while(true){
                                                    if(nxnewtmp.find(p_s[0])==std::string::npos){
                                                        std::stringstream xs;
                                                        xs<<">|<"<<p_s[0]<<":^:"<<p_s[1];
                                                        nxnew=nxnew+xs.str();
                                                        break;
                                                    }
                                                    if(nxnewtmp.substr(nxnewtmp.find(p_s[0])-3).substr(0,3)!=">|<"){
                                                        nxnewtmp=nxnewtmp.substr(nxnewtmp.find(p_s[0]));
                                                        if(nxnewtmp.find(">|<")!=std::string::npos){
                                                            nxnewtmp=nxnewtmp.substr(nxnewtmp.find(">|<")+3);
                                                            continue;
                                                        }
                                                        break;
                                                    }
                                                    if(nxnewtmp.substr(nxnewtmp.find(p_s[0])+p_s[0].length()).substr(0,3)!=":^:"){
                                                        nxnewtmp=nxnewtmp.substr(nxnewtmp.find(p_s[0]));
                                                        if(nxnewtmp.find(">|<")!=std::string::npos){
                                                            nxnewtmp=nxnewtmp.substr(nxnewtmp.find(">|<")+3);
                                                            continue;
                                                        }
                                                        break;
                                                    }
                                                    nxnewtmp=nxnewtmp.substr(nxnewtmp.find(p_s[0]));
                                                    exist=true;
                                                    break;
                                                }
                                                if(exist){
                                                    std::stringstream xs;
                                                    xs<<nxnew.substr(0,nxnew.find(nxnewtmp))<<p_s[0]<<":^:"<<p_s[1];
                                                    if(nxnewtmp.find(">|<")!=std::string::npos){
                                                        xs<<nxnewtmp.substr(nxnewtmp.find(">|<"));
                                                    }
                                                    nxnew=xs.str();
                                                }
                                            }else{
                                                std::stringstream xs;
                                                xs<<">|<"<<p_s[0]<<":^:"<<p_s[1];
                                                nxnew=nxnew+xs.str();
                                            }
                                        }
                                        step=0;
                                        p_0=0;
                                        p_1=0;
                                        p_s[0]=prms[i];
                                        p_s[1]="";
                                    }
                                }
                                bool exist=false;
                                if(nxnew.find(p_s[0])!=std::string::npos){
                                    std::string nxnewtmp=nxnew;
                                    while(true){
                                        if(nxnewtmp.find(p_s[0])==std::string::npos){
                                            std::stringstream xs;
                                            xs<<">|<"<<p_s[0]<<":^:"<<p_s[1];
                                            nxnew=nxnew+xs.str();                                         
                                            break;
                                        }
                                        if(nxnewtmp.substr(nxnewtmp.find(p_s[0])-3).substr(0,3)!=">|<"){
                                            nxnewtmp=nxnewtmp.substr(nxnewtmp.find(p_s[0]));
                                            if(nxnewtmp.find(">|<")!=std::string::npos){
                                                nxnewtmp=nxnewtmp.substr(nxnewtmp.find(">|<")+3);
                                                continue;
                                            }
                                            break;
                                        }
                                        if(nxnewtmp.substr(nxnewtmp.find(p_s[0])+p_s[0].length()).substr(0,3)!=":^:"){
                                            nxnewtmp=nxnewtmp.substr(nxnewtmp.find(p_s[0]));
                                            if(nxnewtmp.find(">|<")!=std::string::npos){
                                                nxnewtmp=nxnewtmp.substr(nxnewtmp.find(">|<")+3);
                                                continue;
                                            }
                                            break;
                                        }
                                        nxnewtmp=nxnewtmp.substr(nxnewtmp.find(p_s[0]));
                                        exist=true;
                                        break;
                                    }
                                    if(exist){
                                        std::stringstream xs;
                                        xs<<nxnew.substr(0,nxnew.find(nxnewtmp))<<p_s[0]<<":^:"<<p_s[1];
                                        if(nxnewtmp.find(">|<")!=std::string::npos){
                                            xs<<nxnewtmp.substr(nxnewtmp.find(">|<"));
                                        }
                                        nxnew=xs.str();
                                    }
                                }else{
                                    std::stringstream xs;
                                    xs<<">|<"<<p_s[0]<<":^:"<<p_s[1];
                                    nxnew=nxnew+xs.str();
                                }
                                glt[1]+=nxnew;
                                break;
                            }
                        }
                        glt[1]+=sxetmp.substr(sxetmp.find("^<>"));
                        glt[0]=invr.substr(0,invr.find(sxe));
                        glt[2]=invr.substr(invr.find(sxe));
                        glt[2]=glt[2].substr(glt[2].find("!#!")+3);
                        std::ofstream outputFile("bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                        outputFile << bfr.str()<<glt[0]<<glt[1]<<glt[2]<<afr;
                        outputFile.close();
                        break;
                    }
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }
            }
        }
    }else if(command=="edit_param_array"){
        if(argc!=7){
            std::cout << "Usage: bound edit_param_array <NameTable> <Identification> <name_column_array> <base_identification> <user:^:DarkSouls_23>|<comment:^:Hello>" << std::endl;
            return -1;
        }
        std::string tableName=argv[2];
        if(!std::filesystem::exists("bound/db/tables/"+tableName)){
            std::cout << "This table not exist" << std::endl;
            std::string wis="bound/db/tables/"+tableName+"/append.bd";
            remove(wis.c_str());
            return -1;
        }
        lock();
        std::ifstream aaq("bound/db/tables/"+tableName+"/find.bd");
        std::stringstream buffer;
        buffer << aaq.rdbuf();
        std::string prty = buffer.str();
        aaq.close();
        std::stringstream ar;
        ar<<argv[3];
        ar<<"<!>";
        size_t nt=prty.find(ar.str());
        if(nt==std::string::npos){
            std::cout<<"This identification not exists"<<std::endl;
            remove("bound/db/tables/lock.bd");
            return 1;
        }else{
            std::string conttmp=prty;
            while(true){
                size_t cn=conttmp.find(ar.str());
                if(cn==std::string::npos){
                    std::cout<<"This identification not exists"<<std::endl;
                    remove("bound/db/tables/lock.bd");
                    return 1;
                }
                std::string se=conttmp.substr(cn-3);
                se=se.substr(0,3);
                std::string fr=conttmp.substr(cn);
                size_t mrn=fr.find("<!>");
                if(se!=">|<"&&se!="<!>"){
                    conttmp=fr.substr(mrn+3);
                    continue;
                }
                if(fr.substr(0,mrn)!=argv[3]){
                    conttmp=fr.substr(mrn+3);
                    continue;
                }
                while(true){
                    int sz=0;
                    int point=0;
                    std::vector<char> seqn={'>','|','<'};
                    while(true){
                        if(seqn[point]==conttmp.substr(cn-sz)[0]){
                            point=point+1;
                            if(point==3){
                                conttmp=conttmp.substr(cn-sz);
                                break;
                            }
                        }else{
                            point=0;
                        }
                        sz=sz+1;
                    }
                    conttmp=conttmp.substr(3);
                    conttmp=conttmp.substr(0,conttmp.find("<|>"));
                    break;
                }
                std::string currensm="";
                std::string path;
                std::string ind;
                for(int spr=0;spr<3;spr++){
                    size_t xnx=conttmp.find(">|<");
                    if(spr==0){
                        size_t xnx=conttmp.find(">|<");
                        currensm=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }else if(spr==1){
                        size_t xnx=conttmp.find(">|<");
                        path=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }else{
                        size_t xnx=conttmp.find(">|<");
                        ind=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }
                }
                std::ifstream ymn("bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                std::stringstream xbuffer;
                xbuffer << ymn.rdbuf();
                std::string invr = xbuffer.str();
                ymn.close();
                std::stringstream bfr;
                bfr<<invr.substr(0,invr.find("<|>")+3);
                invr=invr.substr(invr.find("<|>")+3);
                if(stoi(ind)!=0){
                    for(int i=0;i<stoi(ind);i++){
                        bfr<<invr.substr(0,invr.find("<|>")+3);
                        invr=invr.substr(invr.find("<|>")+3);
                    }
                }
                    
                std::stringstream xs;
                std::string afr=invr.substr(invr.find("<|>"));
                invr=invr.substr(0,invr.find("<|>"));            
                std::stringstream cxc;
                cxc<<argv[4]<<"{>}";
                if(invr.find(cxc.str())==std::string::npos){
                    std::cout<<"This name_column_array not exists";
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }else{
                    std::string tmpinvr=invr;
                    std::vector<std::string>glt={"","",""};
                    while(true){
                        if(tmpinvr.find(cxc.str())==std::string::npos){
                            std::cout<<"This name_column_array not exists"<<std::endl;
                            remove("bound/db/tables/lock.bd");
                            return -1;
                        }
                        std::string sxe=tmpinvr.substr(tmpinvr.find(cxc.str()));
                        std::string se=tmpinvr.substr(tmpinvr.find(cxc.str())-3);
                        if(se.substr(0,3)!=">|<"&&se.substr(0,3)!="<|>"){
                            tmpinvr=tmpinvr.substr(tmpinvr.find(cxc.str())+cxc.str().length());
                            continue;
                        }
                        sxe=sxe.substr(0,sxe.find("{<}")+3);
                        std::string sxetmp=sxe;
                        std::cout<<sxetmp<<std::endl;
                        std::stringstream bs;
                        bs<<"base:^:"<<argv[5];
                        while(true){
                            if(sxetmp.find(bs.str())==std::string::npos){
                                std::cout<<"This base_identification not exists"<<std::endl;
                                remove("bound/db/tables/lock.bd");
                                return -1;
                            }else{
                                std::string seb=sxetmp.substr(sxetmp.find(bs.str())-3);
                                sxetmp=sxetmp.substr(sxetmp.find(bs.str()));
                                seb=seb.substr(0,3);
                                if(seb!="{>}"&&seb!="^<>"){
                                    sxetmp=sxetmp.substr(sxetmp.find(bs.str())+bs.str().length());
                                    continue;
                                }
                                if(sxetmp.substr(bs.str().length()).substr(0,3)!=">|<"&&sxetmp.substr(bs.str().length()).substr(0,3)!="^<>"){
                                    sxetmp=sxetmp.substr(sxetmp.find(bs.str())+bs.str().length());
                                    continue;                                    
                                }
                                glt[1]=sxe.substr(0,sxe.find(sxetmp));
                                std::string prms=argv[6];
                                std::vector<char>v_0={':','^',':'};
                                int p_0=0;
                                std::vector<char>v_1={'>','|','<'};
                                int p_1=0;
                                std::vector<std::string>p_s={"",""};
                                int step=0;
                                std::string nxnew=sxetmp.substr(0,sxetmp.find("^<>"));
                                for(int i=0;i<prms.length();i++){
                                    if(step==0){
                                        p_s[0]+=prms[i];
                                        if(prms[i]==v_0[p_0]){
                                            p_0+=1;
                                            if(p_0==3){
                                                p_s[0]=p_s[0].substr(0,p_s[0].find(":^:"));
                                                step=1;
                                            }
                                        }
                                    }else if(step==1){
                                        p_s[1]+=prms[i];
                                        if(prms[i]==v_1[p_1]){
                                            p_1+=1;
                                            if(p_1==3){
                                                p_s[1]=p_s[1].substr(0,p_s[1].find(">|<"));
                                                step=2;
                                            }
                                        }
                                    }else if(step==2){
                                        if(p_s[0]!="base"){
                                            bool exist=false;
                                            if(nxnew.find(p_s[0])!=std::string::npos){
                                                std::string nxnewtmp=nxnew;
                                                
                                                while(true){
                                                    if(nxnewtmp.find(p_s[0])==std::string::npos){
                                                        std::stringstream xs;
                                                        xs<<">|<"<<p_s[0]<<":^:"<<p_s[1];
                                                        nxnew=nxnew+xs.str();
                                                        break;
                                                    }
                                                    if(nxnewtmp.substr(nxnewtmp.find(p_s[0])-3).substr(0,3)!=">|<"){
                                                        nxnewtmp=nxnewtmp.substr(nxnewtmp.find(p_s[0]));
                                                        if(nxnewtmp.find(">|<")!=std::string::npos){
                                                            nxnewtmp=nxnewtmp.substr(nxnewtmp.find(">|<")+3);
                                                            continue;
                                                        }
                                                        break;
                                                    }
                                                    if(nxnewtmp.substr(nxnewtmp.find(p_s[0])+p_s[0].length()).substr(0,3)!=":^:"){
                                                        nxnewtmp=nxnewtmp.substr(nxnewtmp.find(p_s[0]));
                                                        if(nxnewtmp.find(">|<")!=std::string::npos){
                                                            nxnewtmp=nxnewtmp.substr(nxnewtmp.find(">|<")+3);
                                                            continue;
                                                        }
                                                        break;
                                                    }
                                                    nxnewtmp=nxnewtmp.substr(nxnewtmp.find(p_s[0]));
                                                    exist=true;
                                                    break;
                                                }
                                                if(exist){
                                                    std::stringstream xs;
                                                    xs<<nxnew.substr(0,nxnew.find(nxnewtmp))<<p_s[0]<<":^:"<<p_s[1];
                                                    if(nxnewtmp.find(">|<")!=std::string::npos){
                                                        xs<<nxnewtmp.substr(nxnewtmp.find(">|<"));
                                                    }
                                                    nxnew=xs.str();
                                                }
                                            }else{
                                                std::stringstream xs;
                                                xs<<">|<"<<p_s[0]<<":^:"<<p_s[1];
                                                nxnew=nxnew+xs.str();
                                            }
                                        }
                                        step=0;
                                        p_0=0;
                                        p_1=0;
                                        p_s[0]=prms[i];
                                        p_s[1]="";
                                    }
                                }
                                bool exist=false;
                                if(nxnew.find(p_s[0])!=std::string::npos){
                                    std::string nxnewtmp=nxnew;
                                    while(true){
                                        if(nxnewtmp.find(p_s[0])==std::string::npos){
                                            std::stringstream xs;
                                            xs<<">|<"<<p_s[0]<<":^:"<<p_s[1];
                                            nxnew=nxnew+xs.str();                                         
                                            break;
                                        }
                                        if(nxnewtmp.substr(nxnewtmp.find(p_s[0])-3).substr(0,3)!=">|<"){
                                            nxnewtmp=nxnewtmp.substr(nxnewtmp.find(p_s[0]));
                                            if(nxnewtmp.find(">|<")!=std::string::npos){
                                                nxnewtmp=nxnewtmp.substr(nxnewtmp.find(">|<")+3);
                                                continue;
                                            }
                                            break;
                                        }
                                        if(nxnewtmp.substr(nxnewtmp.find(p_s[0])+p_s[0].length()).substr(0,3)!=":^:"){
                                            nxnewtmp=nxnewtmp.substr(nxnewtmp.find(p_s[0]));
                                            if(nxnewtmp.find(">|<")!=std::string::npos){
                                                nxnewtmp=nxnewtmp.substr(nxnewtmp.find(">|<")+3);
                                                continue;
                                            }
                                            break;
                                        }
                                        nxnewtmp=nxnewtmp.substr(nxnewtmp.find(p_s[0]));
                                        exist=true;
                                        break;
                                    }
                                    if(exist){
                                        std::stringstream xs;
                                        xs<<nxnew.substr(0,nxnew.find(nxnewtmp))<<p_s[0]<<":^:"<<p_s[1];
                                        if(nxnewtmp.find(">|<")!=std::string::npos){
                                            xs<<nxnewtmp.substr(nxnewtmp.find(">|<"));
                                        }
                                        nxnew=xs.str();
                                    }
                                }else{
                                    std::stringstream xs;
                                    xs<<">|<"<<p_s[0]<<":^:"<<p_s[1];
                                    nxnew=nxnew+xs.str();
                                }
                                glt[1]+=nxnew;
                                break;
                            }
                        }
                        glt[1]+=sxetmp.substr(sxetmp.find("^<>"));
                        glt[0]=invr.substr(0,invr.find(sxe));
                        glt[2]=invr.substr(invr.find(sxe));
                        glt[2]=glt[2].substr(glt[2].find("{<}")+3);
                        std::ofstream outputFile("bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                        outputFile << bfr.str()<<glt[0]<<glt[1]<<glt[2]<<afr;
                        outputFile.close();
                        break;
                    }
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }
            }
        }
    }else if(command=="removeI_array"){
        if(argc!=6){
            std::cout<<"Usage: bound remove_array <NameTable> <Identification> <name_column_array> <uuid_base>"<<std::endl;
            return -1;
        }
        std::string tableName=argv[2];
        if(!std::filesystem::exists("bound/db/tables/"+tableName)){
            std::cout << "This table not exist" << std::endl;
            std::string wis="bound/db/tables/"+tableName+"/append.bd";
            remove(wis.c_str());
            return -1;
        }
        lock();
        std::ifstream aaq("bound/db/tables/"+tableName+"/find.bd");
        std::stringstream buffer;
        buffer << aaq.rdbuf();
        std::string prty = buffer.str();
        aaq.close();
        std::stringstream ar;
        ar<<argv[3];
        ar<<"<!>";
        size_t nt=prty.find(ar.str());
        if(nt==std::string::npos){
            std::cout<<"This identification not exists"<<std::endl;
            std::string wis="bound/db/tables/"+tableName+"/append.bd";
            remove(wis.c_str());
            remove("bound/db/tables/lock.bd");
            return -1;
        }else{
            std::string conttmp=prty;
            while(true){
                size_t cn=conttmp.find(ar.str());
                if(cn==std::string::npos){
                    std::cout<<"This identification not exists"<<std::endl;
                    std::string wis="bound/db/tables/"+tableName+"/append.bd";
                    remove(wis.c_str());
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }
                std::string se=conttmp.substr(cn-3);
                se=se.substr(0,3);
                std::string fr=conttmp.substr(cn);
                size_t mrn=fr.find("<!>");
                if(se!=">|<"&&se!="<!>"){
                    conttmp=fr.substr(mrn+3);
                    continue;
                }
                if(fr.substr(0,mrn)!=argv[3]){
                    conttmp=fr.substr(mrn+3);
                    continue;
                }
                while(true){
                    int sz=0;
                    int point=0;
                    std::vector<char> seqn={'>','|','<'};
                    while(true){
                        if(seqn[point]==conttmp.substr(cn-sz)[0]){
                            point=point+1;
                            if(point==3){
                                conttmp=conttmp.substr(cn-sz);
                                break;
                            }
                        }else{
                            point=0;
                        }
                        sz=sz+1;
                    }
                    conttmp=conttmp.substr(3);
                    conttmp=conttmp.substr(0,conttmp.find("<|>"));
                    break;
                }
                std::string currensm="";
                std::string path;
                std::string ind;
                for(int spr=0;spr<3;spr++){
                    size_t xnx=conttmp.find(">|<");
                    if(spr==0){
                        size_t xnx=conttmp.find(">|<");
                        currensm=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }else if(spr==1){
                        size_t xnx=conttmp.find(">|<");
                        path=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }else{
                        size_t xnx=conttmp.find(">|<");
                        ind=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }
                }
                std::ifstream ymn("bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                std::stringstream xbuffer;
                xbuffer << ymn.rdbuf();
                std::string invr = xbuffer.str();
                ymn.close();
                std::stringstream bfr;
                bfr<<invr.substr(0,invr.find("<|>")+3);
                invr=invr.substr(invr.find("<|>")+3);
                if(stoi(ind)!=0){
                    for(int i=0;i<stoi(ind);i++){
                        bfr<<invr.substr(0,invr.find("<|>")+3);
                        invr=invr.substr(invr.find("<|>")+3);
                    }
                }
                
                std::stringstream xs;
                std::string afr=invr.substr(invr.find("<|>"));
                invr=invr.substr(0,invr.find("<|>"));            
                std::stringstream cxc;
                cxc<<argv[4]<<"!@!";
                if(invr.find(cxc.str())==std::string::npos){
                    std::cout<<"This name_column_array not exists";
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }else{
                    std::string tmpinvr=invr;
                    std::vector<std::string>glt={"",""};
                    while(true){
                        if(tmpinvr.find(cxc.str())==std::string::npos){
                            std::cout<<"This name_column_array not exists"<<std::endl;
                            remove("bound/db/tables/lock.bd");
                            return -1;
                        }
                        std::string sxe=tmpinvr.substr(tmpinvr.find(cxc.str()));
                        std::string se=tmpinvr.substr(tmpinvr.find(cxc.str())-3);
                        if(se.substr(0,3)!=">|<"&&se.substr(0,3)!="<|>"){
                            tmpinvr=tmpinvr.substr(tmpinvr.find(cxc.str())+cxc.str().length());
                            continue;
                        }
                        sxe=sxe.substr(0,sxe.find("!#!")+3);
                        std::string sxetmp=sxe;
                        std::stringstream bs;
                        bs<<"uuid_base:^:"<<argv[5];
                        while(true){
                            if(sxetmp.find(bs.str())==std::string::npos){
                                std::cout<<"This base_identification not exists"<<std::endl;
                                remove("bound/db/tables/lock.bd");
                                return -1;
                            }else{
                                std::string seb=sxetmp.substr(sxetmp.find(bs.str())-3);
                                sxetmp=sxetmp.substr(sxetmp.find(bs.str()));
                                seb=seb.substr(0,3);
                                if(seb!=">|<"&&seb!="^<>"){
                                    sxetmp=sxetmp.substr(sxetmp.find(bs.str())+bs.str().length());
                                    continue;
                                }

                                if(sxetmp.substr(bs.str().length()).substr(0,3)!=">|<"&&sxetmp.substr(bs.str().length()).substr(0,3)!="^<>"){
                                    sxetmp=sxetmp.substr(sxetmp.find(bs.str())+bs.str().length());
                                    continue;                                    
                                }
                                std::stringstream psp;
                                int baseN=0;
                                std::string sxetm=sxetmp.substr(0,sxetmp.find("!#!"));
                                if(sxe.substr(0,sxe.find(sxetmp)).find("^<>")!=std::string::npos){
                                    std::string mns=sxe.substr(0,sxe.find(sxetmp));
                                    while(true){
                                        mns=mns.substr(mns.find("^<>")+3);
                                        if(mns.find("^<>")==std::string::npos){
                                            break;
                                        }
                                    }
                                    std::string breat=sxe.substr(0,sxe.find(sxetmp));
                                    breat=breat.substr(0,breat.find(mns));
                                    psp<<breat;
                                }else{
                                    psp<<sxe.substr(0,sxe.find("!@!")+3);
                                    }
                                    
                                while(true){
                                    sxetm=sxetm.substr(sxetm.find("^<>")+3);
                                    baseN=baseN+1;
                                
                                    if(sxetm==""){
                                        break;
                                    }
                                    
                                    std::string bssb=sxetm.substr(0,sxetm.find("^<>"));

                                    int nb=stoi(bssb.substr(0,bssb.find(">|<")).substr(bssb.find(":^:")+3))-1;
                                    psp<<"base:^:"<<nb<<bssb.substr(bssb.find(">|<"))<<"^<>";
                                }
                                psp<<"!#!";
                                
                                glt[0]=invr.substr(0,invr.find(sxe));
                                glt[1]=invr.substr(invr.find(sxe));
                                glt[1]=glt[1].substr(glt[1].find("!#!")+3);
                                if(psp.str()==""){
                                    glt[0]=glt[0].substr(0,glt[0].length()-3);
                                }
                                std::ofstream outputFile("bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                                outputFile<< bfr.str()<<glt[0]<<psp.str()<<glt[1]<<afr;
                                outputFile.close();
                                remove("bound/db/tables/lock.bd");
                                
                                return 0;
                            }
                        }
                    }
                }
            }
        }
    }else if(command=="remove_array"){
        if(argc!=6){
            std::cout<<"Usage: bound remove_array <NameTable> <Identification> <name_column_array> <base_identification>"<<std::endl;
            return -1;
        }
        std::string tableName=argv[2];
        if(!std::filesystem::exists("bound/db/tables/"+tableName)){
            std::cout << "This table not exist" << std::endl;
            std::string wis="bound/db/tables/"+tableName+"/append.bd";
            remove(wis.c_str());
            return -1;
        }
        lock();
        std::ifstream aaq("bound/db/tables/"+tableName+"/find.bd");
        std::stringstream buffer;
        buffer << aaq.rdbuf();
        std::string prty = buffer.str();
        aaq.close();
        std::stringstream ar;
        ar<<argv[3];
        ar<<"<!>";
        size_t nt=prty.find(ar.str());
        if(nt==std::string::npos){
            std::cout<<"This identification not exists"<<std::endl;
            std::string wis="bound/db/tables/"+tableName+"/append.bd";
            remove(wis.c_str());
            remove("bound/db/tables/lock.bd");
            return -1;
        }else{
            std::string conttmp=prty;
            while(true){
                size_t cn=conttmp.find(ar.str());
                if(cn==std::string::npos){
                    std::cout<<"This identification not exists"<<std::endl;
                    std::string wis="bound/db/tables/"+tableName+"/append.bd";
                    remove(wis.c_str());
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }
                std::string se=conttmp.substr(cn-3);
                se=se.substr(0,3);
                std::string fr=conttmp.substr(cn);
                size_t mrn=fr.find("<!>");
                if(se!=">|<"&&se!="<!>"){
                    conttmp=fr.substr(mrn+3);
                    continue;
                }
                if(fr.substr(0,mrn)!=argv[3]){
                    conttmp=fr.substr(mrn+3);
                    continue;
                }
                while(true){
                    int sz=0;
                    int point=0;
                    std::vector<char> seqn={'>','|','<'};
                    while(true){
                        if(seqn[point]==conttmp.substr(cn-sz)[0]){
                            point=point+1;
                            if(point==3){
                                conttmp=conttmp.substr(cn-sz);
                                break;
                            }
                        }else{
                            point=0;
                        }
                        sz=sz+1;
                    }
                    conttmp=conttmp.substr(3);
                    conttmp=conttmp.substr(0,conttmp.find("<|>"));
                    break;
                }
                std::string currensm="";
                std::string path;
                std::string ind;
                for(int spr=0;spr<3;spr++){
                    size_t xnx=conttmp.find(">|<");
                    if(spr==0){
                        size_t xnx=conttmp.find(">|<");
                        currensm=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }else if(spr==1){
                        size_t xnx=conttmp.find(">|<");
                        path=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }else{
                        size_t xnx=conttmp.find(">|<");
                        ind=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }
                }
                std::ifstream ymn("bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                std::stringstream xbuffer;
                xbuffer << ymn.rdbuf();
                std::string invr = xbuffer.str();
                ymn.close();
                std::stringstream bfr;
                bfr<<invr.substr(0,invr.find("<|>")+3);
                invr=invr.substr(invr.find("<|>")+3);
                if(stoi(ind)!=0){
                    for(int i=0;i<stoi(ind);i++){
                        bfr<<invr.substr(0,invr.find("<|>")+3);
                        invr=invr.substr(invr.find("<|>")+3);
                    }
                }
                    
                std::stringstream xs;
                std::string afr=invr.substr(invr.find("<|>"));
                invr=invr.substr(0,invr.find("<|>"));            
                std::stringstream cxc;
                cxc<<argv[4]<<"!@!";
                if(invr.find(cxc.str())==std::string::npos){
                    std::cout<<"This name_column_array not exists";
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }else{
                    std::string tmpinvr=invr;
                    std::vector<std::string>glt={"",""};
                    while(true){
                        if(tmpinvr.find(cxc.str())==std::string::npos){
                            std::cout<<"This name_column_array not exists"<<std::endl;
                            remove("bound/db/tables/lock.bd");
                            return -1;
                        }
                        std::string sxe=tmpinvr.substr(tmpinvr.find(cxc.str()));
                        std::string se=tmpinvr.substr(tmpinvr.find(cxc.str())-3);
                        if(se.substr(0,3)!=">|<"&&se.substr(0,3)!="<|>"){
                            tmpinvr=tmpinvr.substr(tmpinvr.find(cxc.str())+cxc.str().length());
                            continue;
                        }
                        sxe=sxe.substr(0,sxe.find("!#!")+3);
                        std::string sxetmp=sxe;
                        std::stringstream bs;
                        bs<<"base:^:"<<argv[5];
                        while(true){
                            if(sxetmp.find(bs.str())==std::string::npos){
                                std::cout<<"This base_identification not exists"<<std::endl;
                                remove("bound/db/tables/lock.bd");
                                return -1;
                            }else{
                                std::string seb=sxetmp.substr(sxetmp.find(bs.str())-3);
                                sxetmp=sxetmp.substr(sxetmp.find(bs.str()));
                                seb=seb.substr(0,3);
                                if(seb!="!@!"&&seb!="^<>"){
                                    sxetmp=sxetmp.substr(sxetmp.find(bs.str())+bs.str().length());
                                    continue;
                                }
                                if(sxetmp.substr(bs.str().length()).substr(0,3)!=">|<"&&sxetmp.substr(bs.str().length()).substr(0,3)!="^<>"){
                                    sxetmp=sxetmp.substr(sxetmp.find(bs.str())+bs.str().length());
                                    continue;                                    
                                }
                                
                                std::stringstream psp;
                                int baseN=0;
                                std::string sxetm=sxetmp.substr(0,sxetmp.find("!#!"));
                                psp<<sxe.substr(0,sxe.find(sxetmp));
                                while(true){
                                    sxetm=sxetm.substr(sxetm.find("^<>")+3);
                                    baseN=baseN+1;
                                    
                                    if(sxetm==""){
                                        break;
                                    }
                                        
                                    std::string bssb=sxetm.substr(0,sxetm.find("^<>"));
                                    int nb=stoi(bssb.substr(0,bssb.find(">|<")).substr(bssb.find(":^:")+3))-1;
                                    psp<<"base:^:"<<nb<<bssb.substr(bssb.find(">|<"))<<"^<>";
                                }
                                psp<<"!#!";
                                glt[0]=invr.substr(0,invr.find(sxe));
                                glt[1]=invr.substr(invr.find(sxe));
                                glt[1]=glt[1].substr(glt[1].find("!#!")+3);
                                if(psp.str()==""){
                                    glt[0]=glt[0].substr(0,glt[0].length()-3);
                                }
                                std::ofstream outputFile("bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                                outputFile << bfr.str()<<glt[0]<<psp.str()<<glt[1]<<afr;
                                outputFile.close();
                                remove("bound/db/tables/lock.bd");
                                
                                return 0;
                            }
                        }
                    }
                }
            }
        }
    }else if(command=="append_array"){
        if(argc<5){
            std::cout << "Usage: bound append_array <NameTable> <Identification> <name_column_array>" << std::endl;
            return -1;
        }

        std::string tableName=argv[2];
        if(!std::filesystem::exists("bound/db/tables/"+tableName)){
            std::cout << "This table not exist" << std::endl;
            std::string wis="bound/db/tables/"+tableName+"/append.bd";
            remove(wis.c_str());
            return -1;
        }
        lock();
        std::ifstream aaq("bound/db/tables/"+tableName+"/find.bd");
        std::stringstream buffer;
        buffer << aaq.rdbuf();
        std::string prty = buffer.str();
        aaq.close();
        std::ifstream appendFile("bound/db/tables/"+tableName+"/append.bd");

        std::stringstream ar;
        ar<<argv[3];
        ar<<"<!>";
        size_t nt=prty.find(ar.str());
        if(nt==std::string::npos){
            std::cout<<"This identification not exists"<<std::endl;
            std::string wis="bound/db/tables/"+tableName+"/append.bd";
            remove(wis.c_str());
            remove("bound/db/tables/lock.bd");
            return -1;
        }else{
            std::string conttmp=prty;
            while(true){
                size_t cn=conttmp.find(ar.str());
                if(cn==std::string::npos){
                    std::cout<<"This identification not exists"<<std::endl;
                    std::string wis="bound/db/tables/"+tableName+"/append.bd";
                    remove(wis.c_str());
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }
                std::string se=conttmp.substr(cn-3);
                se=se.substr(0,3);
                std::string fr=conttmp.substr(cn);
                size_t mrn=fr.find("<!>");
                if(se!=">|<"&&se!="<!>"){
                    conttmp=fr.substr(mrn+3);
                    continue;
                }
                if(fr.substr(0,mrn)!=argv[3]){
                    conttmp=fr.substr(mrn+3);
                    continue;
                }
                while(true){
                    int sz=0;
                    int point=0;
                    std::vector<char> seqn={'>','|','<'};
                    while(true){
                        if(seqn[point]==conttmp.substr(cn-sz)[0]){
                            point=point+1;
                            if(point==3){
                                conttmp=conttmp.substr(cn-sz);
                                break;
                            }
                        }else{
                            point=0;
                        }
                        sz=sz+1;
                    }
                    conttmp=conttmp.substr(3);
                    conttmp=conttmp.substr(0,conttmp.find("<|>"));
                    break;
                }
                std::string currensm="";
                std::string path;
                std::string ind;
                for(int spr=0;spr<3;spr++){
                    size_t xnx=conttmp.find(">|<");
                    if(spr==0){
                        size_t xnx=conttmp.find(">|<");
                        currensm=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }else if(spr==1){
                        size_t xnx=conttmp.find(">|<");
                        path=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }else{
                        size_t xnx=conttmp.find(">|<");
                        ind=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }
                }
                std::ifstream ymn("bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                std::stringstream xbuffer;
                xbuffer << ymn.rdbuf();
                std::string invr = xbuffer.str();
                ymn.close();
                std::stringstream bfr;
                bfr<<invr.substr(0,invr.find("<|>")+3);
                invr=invr.substr(invr.find("<|>")+3);
                if(stoi(ind)!=0){
                    for(int i=0;i<stoi(ind);i++){
                        bfr<<invr.substr(0,invr.find("<|>")+3);
                        invr=invr.substr(invr.find("<|>")+3);
                    }
                }
                    
                std::stringstream xs;
                std::string afr=invr.substr(invr.find("<|>"));
                invr=invr.substr(0,invr.find("<|>"));
                if(invr.find(argv[4])==std::string::npos){
                    std::cout<<"This array not exists"<<std::endl;
                    std::string wis="bound/db/tables/"+tableName+"/append.bd";
                    remove(wis.c_str());
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }
                size_t arr=invr.find(argv[4]);
                std::vector<std::string>glt={"","",""};
                glt[0]=invr.substr(0,arr);
                std::string tmpchild=invr.substr(arr);
                if(tmpchild.find(":^:")==std::string::npos){
                    std::cout<<"This array not exists"<<std::endl;
                    std::string wis="bound/db/tables/"+tableName+"/append.bd";
                    remove(wis.c_str());
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }
                size_t ar0=tmpchild.find(":^:");
                if(tmpchild.substr(ar0+3)[0]!='['){
                    std::cout<<"This array not exists"<<std::endl;
                    std::string wis="bound/db/tables/"+tableName+"/append.bd";
                    remove(wis.c_str());
                    remove("bound/db/tables/lock.bd");
                    return -1;
                } 
                if(tmpchild.substr(0,ar0)!=argv[4]){
                    std::cout<<"This array not exists"<<std::endl;
                    std::string wis="bound/db/tables/"+tableName+"/append.bd";
                    remove(wis.c_str());
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }
                std::string axrr=tmpchild.substr(ar0+3);
                std::string axtemp=axrr.substr(1);
                size_t opn=axtemp.find('[');
                size_t art=axtemp.find(']');
                size_t arrl=axrr.find(']');
                while(true){
                    if(opn!=std::string::npos){
                        arrl=axrr.find(']',arrl+1);
                        opn=axtemp.find('[',opn+1);
                        continue;
                    }
                    break;
                }
                glt[1]=tmpchild.substr(0,ar0+3);

                glt[2]=axrr.substr(arrl+1);
                axrr=axrr.substr(0,arrl+1);
                std::stringstream arend;
                arend<<axrr.substr (0,axrr.length()-1);
                std::stringstream buffer;
                buffer << appendFile.rdbuf();
                std::string append = buffer.str();
                if(append.find("<|>")!=std::string::npos){
                    std::string wis="bound/db/tables/"+tableName+"/append.bd";
                    remove(wis.c_str());
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }if(append.find(">|<")!=std::string::npos){
                    std::string wis="bound/db/tables/"+tableName+"/append.bd";
                    remove(wis.c_str());
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }if(append.find("<!>")!=std::string::npos){
                    std::string wis="bound/db/tables/"+tableName+"/append.bd";
                    remove(wis.c_str());
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }
                appendFile.close();

                std::string gv=append;
                if(gv[0]!='['||gv[gv.size()-1]!=']'){
                    std::cout<<"new_data need be array"<<std::endl;
                    std::string wis="bound/db/tables/"+tableName+"/append.bd";
                    remove(wis.c_str());
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }
                //Here check if arend is [ empty or exist some value as eg: ['blahblahbl','thinkingbo',{"ss":"etc"}
                if(arend.str()!="["){
                    arend<<",";
                }
                gv=gv.substr(1);
                gv=gv.substr(0,gv.size()-1);
                arend<<gv;
                arend<<"]";
                std::string nwparam=bfr.str()+glt[0]+glt[1]+arend.str()+glt[2]+afr;
                std::ofstream outputFile("bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                outputFile << nwparam;
                outputFile.close();
                std::cout<<"Array updated"<<std::endl;
                std::string wis="bound/db/tables/"+tableName+"/append.bd";
                remove(wis.c_str());
                remove("bound/db/tables/lock.bd");
                return 0;                                           
            }
            std::cout<<"This identification not exists"<<std::endl;
            std::string wis="bound/db/tables/"+tableName+"/append.bd";
            remove(wis.c_str());
            remove("bound/db/tables/lock.bd");
            return -1;
        }
    }else if(command=="pop_arrayKV"){
        if(argc<6){
            std::cout << "Usage: bound pop_arrayKV <NameTable> <Identification> <column_array> <value_delete>" << std::endl;
            return 1;
        }
        std::string tableName=argv[2];
        std::string tb=argv[3];
        std::string tzb=argv[4];
        std::string txb=argv[5];
        
        if(tableName.find("<|>")!=std::string::npos||tableName.find(">|<")!=std::string::npos||tableName.find("<!>")!=std::string::npos||
        tb.find("<|>")!=std::string::npos||tb.find(">|<")!=std::string::npos||tb.find("<!>")!=std::string::npos||
        tzb.find("<|>")!=std::string::npos||tzb.find(">|<")!=std::string::npos||tzb.find("<!>")!=std::string::npos||
        txb.find("<|>")!=std::string::npos||txb.find(">|<")!=std::string::npos||txb.find("<!>")!=std::string::npos){
            return -1;
        }
        if(!std::filesystem::exists("bound/db/tables/"+tableName)){
            std::cout << "This table not exist" << std::endl;
            return 1;
        }
        if(!std::filesystem::exists("bound/db/tables/"+tableName+"/find.bd")){
            std::cout<<"This identification not exists"<<std::endl;
            return 1;
        }
        lock();
        std::ifstream aaq("bound/db/tables/"+tableName+"/find.bd");
        std::stringstream buffer;
        buffer << aaq.rdbuf();
        std::string prty = buffer.str();
        aaq.close();
        std::stringstream ar;
        ar<<argv[3];
        ar<<"<!>";
        size_t nt=prty.find(ar.str());
        if(nt==std::string::npos){
            std::cout<<"This identification not exists"<<std::endl;
            remove("bound/db/tables/lock.bd");
            return -1;
        }else{
            std::string conttmp=prty;
            while(true){
                size_t cn=conttmp.find(ar.str());
                if(cn==std::string::npos){
                    std::cout<<"This identification not exists"<<std::endl;
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }
                std::string se=conttmp.substr(cn-3);
                se=se.substr(0,3);
                std::string fr=conttmp.substr(cn);
                size_t mrn=fr.find("<!>");
                if(se!=">|<"&&se!="<!>"){
                    conttmp=fr.substr(mrn+3);
                    continue;

                }
                if(fr.substr(0,mrn)!=argv[3]){
                    conttmp=fr.substr(mrn+3);
                    continue;
                }
                while(true){
                    int sz=0;
                    int point=0;
                    std::vector<char> seqn={'>','|','<'};
                    while(true){
                        if(seqn[point]==conttmp.substr(cn-sz)[0]){
                            point=point+1;
                            if(point==3){
                                conttmp=conttmp.substr(cn-sz);
                                break;
                            }
                        }else{
                            point=0;
                        }
                        sz=sz+1;
                    }
                    conttmp=conttmp.substr(3);
                    conttmp=conttmp.substr(0,conttmp.find("<|>"));
                    break;
                }
                std::string currensm="";
                std::string path;
                std::string ind;
                for(int spr=0;spr<3;spr++){
                    size_t xnx=conttmp.find(">|<");
                    if(spr==0){
                        size_t xnx=conttmp.find(">|<");
                        currensm=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }else if(spr==1){
                        size_t xnx=conttmp.find(">|<");
                        path=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }else{
                        size_t xnx=conttmp.find(">|<");
                        ind=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }
                }
                std::ifstream ymn("bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                std::stringstream xbuffer;
                xbuffer << ymn.rdbuf();
                std::string invr = xbuffer.str();
                ymn.close();
                std::stringstream bfr;
                bfr<<invr.substr(0,invr.find("<|>")+3);
                invr=invr.substr(invr.find("<|>")+3);
                if(stoi(ind)!=0){
                    for(int i=0;i<stoi(ind);i++){
                        bfr<<invr.substr(0,invr.find("<|>")+3);
                        invr=invr.substr(invr.find("<|>")+3);
                    }
                }
                std::stringstream xs;
                std::string afr=invr.substr(invr.find("<|>"));
                invr=invr.substr(0,invr.find("<|>"));
                if(invr.find(argv[4])==std::string::npos){
                    std::cout<<"This array not existzs"<<std::endl;
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }
                size_t arr=invr.find(argv[4]);
                std::vector<std::string>glt={"","",""};
                glt[0]=invr.substr(0,arr);
                std::string tmpchild=invr.substr(arr);
                if(tmpchild.find(":^:")==std::string::npos){
                    std::cout<<"This array not existxs"<<std::endl;
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }
                size_t ar0=tmpchild.find(":^:");
                if(tmpchild.substr(ar0+3)[0]!='['){
                    std::cout<<"This array not existws"<<std::endl;
                    remove("bound/db/tables/lock.bd");
                    return -1;
                } 
                if(tmpchild.substr(0,ar0)!=argv[4]){
                    std::cout<<"This array not existfs"<<std::endl;
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }
                std::string axrr=tmpchild.substr(ar0+3);
                std::string axtemp=axrr.substr(1);
                size_t opn=axtemp.find('[');
                size_t art=axtemp.find(']');
                size_t arrl=axrr.find(']');
                while(true){
                    if(opn!=std::string::npos){
                        arrl=axrr.find(']',arrl+1);
                        opn=axtemp.find('[',opn+1);
                        continue;
                    }
                    break;
                }
                glt[1]=tmpchild.substr(0,ar0+3);
                axrr=axrr.substr(0,arrl+1);
                std::string ss=argv[5];
                std::stringstream xss;
                if(axrr.find(argv[5])==std::string::npos){
                    std::cout<<"This param not exists"<<std::endl;
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }
                if(axrr.substr(0,axrr.find(argv[5]))=="["){
                    if(axrr.substr(axrr.find(argv[5])+ss.length())[0]==','){
                        xss<<axrr.substr(0,axrr.find(argv[5]));
                        axrr=axrr.substr(axrr.find(argv[5])+ss.length()+1);
                    }else{
                        axrr="[]";
                    }
                }else{
                    std::string ilvsl=axrr.substr(0,axrr.find(argv[5]));
                    if(ilvsl.substr(ilvsl.length()-1)==","){
                        xss<<axrr.substr(0,axrr.find(argv[5])-1);
                        axrr=axrr.substr(axrr.find(argv[5])+ss.length());
                    }
                }
                xss<<axrr;
                std::stringstream bls;
                bls<<bfr.str()<<glt[0]<<glt[1]<<xss.str()<<afr;
                std::ofstream outputFile("bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                outputFile << bls.str();
                outputFile.close();
                remove("bound/db/tables/lock.bd");
                return 0;
            }
        }
    }else if(command=="update"){
        if(argc<5){
            std::cout << "Usage: bound update <NameTable> <Identification> <existent_parameter1:^:newdata1>|<new_parameter1:^:newdata2...>" << std::endl;
            return -1;
        }

        std::string tableName=argv[2];
        std::string tb=argv[3];        
        if(tableName.find("<|>")!=std::string::npos||tableName.find(">|<")!=std::string::npos||tableName.find("<!>")!=std::string::npos||
        tb.find("<|>")!=std::string::npos||tb.find(">|<")!=std::string::npos||tb.find("<!>")!=std::string::npos){
            return -1;
        }

        if(!std::filesystem::exists("bound/db/tables/"+tableName)){
            std::cout << "This table not exist" << std::endl;
            return -1;
        }
        lock();
        std::ifstream aaq("bound/db/tables/"+tableName+"/find.bd");
        std::stringstream buffer;
        buffer << aaq.rdbuf();
        std::string prty = buffer.str();
        aaq.close();
        std::stringstream ar;
        ar<<argv[3];
        ar<<"<!>";
        size_t nt=prty.find(ar.str());
        if(nt==std::string::npos){
            std::cout<<"This identification not exists"<<std::endl;
            remove("bound/db/tables/lock.bd");
            return -1;
        }else{
            std::string conttmp=prty;
            
            while(true){
                size_t cn=conttmp.find(ar.str());
                if(cn==std::string::npos){
                    std::cout<<"This identification not exists"<<std::endl;
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }
                
                std::string se=conttmp.substr(cn-3);
                se=se.substr(0,3);
                std::string fr=conttmp.substr(cn);
                size_t mrn=fr.find("<!>");
                if(se!=">|<"&&se!="<!>"){
                    conttmp=fr.substr(mrn+3);
                    continue;
                }
                if(fr.substr(0,mrn)!=argv[3]){
                    conttmp=fr.substr(mrn+3);
                    continue;
                }
                while(true){
                    int sz=0;
                    int point=0;
                    std::vector<char> seqn={'>','|','<'};
                    while(true){
                        if(seqn[point]==conttmp.substr(cn-sz)[0]){
                            point=point+1;
                            if(point==3){
                                conttmp=conttmp.substr(cn-sz);
                                break;
                            }
                        }else{
                            point=0;
                        }
                        sz=sz+1;
                    }
                    conttmp=conttmp.substr(3);
                    conttmp=conttmp.substr(0,conttmp.find("<|>"));
                    break;
                }
                std::string currensm="";
                std::string path;
                std::string ind;
                std::string dns=conttmp;
                for(int spr=0;spr<3;spr++){
                    size_t xnx=conttmp.find(">|<");
                    if(spr==0){
                        size_t xnx=conttmp.find(">|<");
                        currensm=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }else if(spr==1){
                        size_t xnx=conttmp.find(">|<");
                        path=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }else{
                        size_t xnx=conttmp.find(">|<");
                        ind=conttmp.substr(0,xnx);
                        conttmp=conttmp.substr(xnx+3);
                    }
                }
                std::ifstream ymn("bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                std::stringstream bufferx;
                bufferx << ymn.rdbuf();
                std::string invr = bufferx.str();
                ymn.close();
                std::stringstream bfre;
                bfre<<invr.substr(0,invr.find("<|>")+3);
                invr=invr.substr(invr.find("<|>")+3);
                if(stoi(ind)!=0){
                    for(int i=0;i<stoi(ind);i++){
                        bfre<<invr.substr(0,invr.find("<|>")+3);
                        invr=invr.substr(invr.find("<|>")+3);
                    }
                }
                std::vector<std::string>vec={"",""};
                std::stringstream xs;
                std::string afr=invr.substr(invr.find("<|>"));
                vec[0]=bfre.str();
                vec[1]=afr;
                invr=invr.substr(0,invr.find("<|>"));
                std::ifstream inputFile("bound/db/tables/"+tableName+"/config.bd");
                std::stringstream buffer;
                buffer << inputFile.rdbuf();
                std::string fileContent = buffer.str();
                inputFile.close();
                size_t colsize=fileContent.find("identifier");
                std::string col=fileContent.substr(colsize);
                size_t colend=fileContent.find("}",colsize);
                std::string arg=argv[4];
                
                std::vector<std::string> dict=parseKV(arg);
                bool upfind=false;
                for(const auto&i : dict){
                    std::cout<<i<<std::endl;
                    size_t arr=invr.find(i);
                    std::vector<std::string>glt={"","",""};

                    std::string cld=fileContent.substr(colsize+12,(colend-12)-colsize-3);
                    size_t w=i.find(":^:");
                    while(true){
                        if(cld.find("<!>")==std::string::npos){
                            break;
                        }
                        if(i.substr(0,w)==cld.substr(0,cld.find("<!>"))){
                            std::stringstream ar;
                            ar<<i.substr(w+3)<<"<!>";
                            conttmp=prty;
                            bool exist=true;
                            std::string inv=invr;
                            while(true){
                                size_t cn=conttmp.find(ar.str());
                                if(cn==std::string::npos){
                                    std::stringstream bk;
                                    bk<<prty.substr(0,prty.find(dns));
                                    std::string end=prty.substr(prty.find(dns));
                                    end=end.substr(end.find("<|>"));
                                    if(inv.find(i.substr(0,w))==std::string::npos){
                                        prty=bk.str()+dns+ar.str()+end;
                                        dns=dns+ar.str();
                                    }else{
                                        size_t sds=inv.find(i.substr(0,w));
                                        std::string stn=inv.substr(sds);
                                        if(stn.substr(0,stn.find(":^:"))!=i.substr(0,w)){
                                            stn=stn.substr(stn.find(":^:"));
                                            if(stn.find(">|<")!=std::string::npos){
                                                inv=stn.substr(stn.find(">|<")+3);
                                            }
                                            continue;
                                        }else{
                                            std::string g=stn.substr(stn.find(":^:")+3);
                                            if(g.find(">|<")!=std::string::npos){
                                                g=g.substr(0,g.find(">|<"));
                                            }
                                            std::stringstream ss;
                                            ss<<dns.substr(0,dns.find(g))<<i.substr(w+3);
                                            std::string dnstmp=dns.substr(dns.find(g));
                                            ss<<dnstmp.substr(dnstmp.find("<!>"))<<end;
                                            bk<<ss.str();
                                            std::string bfs=dns.substr(0,dns.find(g));
                                            dns=dns.substr(dns.find(g));
                                            dns=dns.substr(dns.find("<!>"));
                                            prty=bk.str();
                                            dns=bfs+i.substr(w+3)+dns;
                                        }
                                    }

                                    exist=false;
                                    break;
                                }
                                inv="";
                                std::string se=conttmp.substr(cn-3);
                                se=se.substr(0,3);
                                std::string fr=conttmp.substr(cn);
                                size_t mrn=fr.find("<!>");
                                if(se!=">|<"&&se!="<!>"){
                                    conttmp=fr.substr(mrn+3);
                                    continue;
                                }
                                if(fr.substr(0,mrn)!=i.substr(w+3)){
                                    conttmp=fr.substr(mrn+3);
                                    continue;
                                }
                                fr="";
                                break;
                            }
                            if(exist){
                                std::cout<<"Identification: "<<i<<", already exists"<<std::endl;
                                remove("bound/db/tables/lock.bd");
                                return -1;
                            }
                            upfind=true;
                        }
                        cld=cld.substr(cld.find("<!>")+3);
                    }
                    if(invr.find(i.substr(0,w))==std::string::npos){
                        invr+=">|<"+i;
                    }else{
                        size_t vle=invr.find(i.substr(0,w));
                        std::string chk=invr.substr(vle);
                        std::string bfr=invr.substr(0,vle);

                        size_t wo=chk.find(":^:");
                        if(chk.substr(0,wo)!=i.substr(0,w)){
                            invr+=">|<"+i;
                        }else{
                            size_t w=invr.find(chk.substr(0,wo));
                            std::string aftr=invr.substr(w);
                            size_t fts=aftr.find(":^:");
                            aftr=aftr.substr(fts);
                            if(aftr.find(">|<")!=std::string::npos){
                                size_t ftr=aftr.find(">|<");
                                aftr=aftr.substr(ftr);
                            }else{
                                aftr="";
                            }
                            std::stringstream ss;
                            ss<<bfr<<i<<aftr;
                            invr=ss.str();
                        }
                    }
                }
                if(upfind){
                    std::ofstream outputFile("bound/db/tables/"+tableName+"/find.bd");
                    outputFile << prty;
                    outputFile.close();
                }
                std::stringstream sn;
                sn<<vec[0]<<invr<<vec[1];
                std::ofstream outputFile("bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                outputFile<<sn.str();
                outputFile.close();
                break;
            }
            remove("bound/db/tables/lock.bd");
            return 0;
        }
    }else if(command=="delete"){
        if(argc<4){
            std::cout << "Usage: bound delete <NameTable> <Identifier>" << std::endl;
            return -1;
        }
        std::string tableName=argv[2];
        std::string tb=argv[3];
        
        if(tableName.find("<|>")!=std::string::npos||tableName.find(">|<")!=std::string::npos||tableName.find("<!>")!=std::string::npos||
        tb.find("<|>")!=std::string::npos||tb.find(">|<")!=std::string::npos||tb.find("<!>")!=std::string::npos){
            return -1;
        }

        if(!std::filesystem::exists("bound/db/tables/"+tableName)){
            std::cout << "This table not exist" << std::endl;
            return -1;
        }
        lock();
        std::ifstream aaq("bound/db/tables/"+tableName+"/find.bd");
        std::stringstream buffer;
        buffer << aaq.rdbuf();
        std::string prty = buffer.str();
        aaq.close();
        std::stringstream ar;
        ar<<argv[3];
        ar<<"<!>";
        size_t nt=prty.find(ar.str());
        if(nt==std::string::npos){
            std::cout<<"This identification not exists"<<std::endl;
            remove("bound/db/tables/lock.bd");
            return -1;
        }else{
            std::string conttmp=prty;
            while(true){
                size_t cn=conttmp.find(ar.str());
                if(cn==std::string::npos){
                    std::cout<<"This identification not exists"<<std::endl;
                    remove("bound/db/tables/lock.bd");
                    return -1;
                }
                std::string se=conttmp.substr(cn-3);
                se=se.substr(0,3);
                std::string fr=conttmp.substr(cn);
                size_t mrn=fr.find("<!>");
                if(se!=">|<"&&se!="<!>"){
                    conttmp=fr.substr(mrn+3);
                    continue;
                }
                if(fr.substr(0,mrn)!=argv[3]){
                    conttmp=fr.substr(mrn+3);
                    continue;
                }
                
                while(true){
                    int sz=0;
                    int point=0;
                    std::vector<char> seqn={'>','|','<'};
                    while(true){
                        if(seqn[point]==conttmp.substr(cn-sz)[0]){
                            point=point+1;
                            if(point==3){
                                conttmp=conttmp.substr(cn-sz);
                                break;
                            }
                        }else{
                            point=0;
                        }
                        sz=sz+1;
                    }
                    conttmp=conttmp.substr(3);
                    break;
                }
                std::string ind=conttmp.substr(conttmp.find(">|<"));
                std::string currsm=conttmp.substr(0,conttmp.find(">|<"));
                ind=ind.substr(3);                
                ind=ind.substr(0,ind.find(">|<"));
                std::stringstream findNw;
                findNw<<prty.substr(0,prty.find(conttmp));
                std::string old=prty.substr(0,prty.find(conttmp));
                std::string last=old;
                bool first=false;
                if(old!="<|>"){
                    int sz=0;
                    int point=0;
                    bool zfirst=true;
                    std::vector<char> seqn={'>','|','<'};
                    while(true){
                        if(seqn[point]==old.substr(old.size()-sz)[0]){
                            point=point+1;
                            if(point==3){
                                last=old.substr(old.size()-sz);
                                if(zfirst){
                                    sz=sz+1;
                                    point=0;
                                    zfirst=false;
                                    continue;
                                }
                                break;
                            }
                        }else{
                            point=0;
                        }
                        sz=sz+1;
                    }                     
                }else{
                    first=true;
                }
                if(!first){
                    std::string indbefore=last.substr(last.find(">|<")+3);
                    indbefore=indbefore.substr(0,indbefore.find(">|<"));
                    if(indbefore!=ind){
                        first=true;
                    }
                }

                int qtyAfter=0;
                while(true){
                    if(conttmp.substr(conttmp.find("<|>"))=="<|>"){
                        conttmp="";
                        break;
                    }
                    
                    conttmp=conttmp.substr(conttmp.find("<|>")+3);
                    std::string vv=conttmp.substr(conttmp.find(">|<")+3);
                    vv=vv.substr(0,vv.find(">|<"));
                    
                    if(ind!=vv){
                        break;
                    }
                    qtyAfter=qtyAfter+1;
                    std::string ffd=conttmp.substr(0,conttmp.find("<|>"));
                    std::string start=ffd.substr(0,ffd.find(">|<"));
                    ffd=ffd.substr(ffd.find(">|<")+3);
                    std::string startf=ffd.substr(0,ffd.find(">|<"));
                    std::string indx=ffd.substr(ffd.find(">|<")+3);
                    std::string idz=indx.substr(0,indx.find(">|<"));
                    
                    int v=stoi(idz);
                    v=v-1;
                    findNw<<start<<">|<"<<startf<<">|<"<<std::to_string(v)<<">|<"<<indx.substr(indx.find(">|<")+3)<<"<|>";
                }
                findNw<<conttmp;
                std::stringstream kmp;
                kmp<<"bound/db/tables/"<<tableName<<"/"<<currsm<<"/"<<ind<<".bd";
                std::ifstream conf("bound/db/tables/"+tableName+"/config.bd");
                std::stringstream *by = new std::stringstream;
                (*by)<<conf.rdbuf();
                conf.close();
                std::string config=(*by).str();
                by->str(std::string());

                size_t startPos=config.find(":",config.find("currensm"));        
                size_t endPos=config.find("<|>",startPos);
                size_t startPos0=config.find(":",config.find("qty"));        
                size_t endPos0=config.find("<|>",startPos0);
                size_t startPos1=config.find(":",config.find("indza"));        
                size_t endPos1=config.find("<|>",startPos1);
                size_t startPos2=config.find(":",config.find("sizeKb"));        
                size_t endPos2=config.find("<|>",startPos2);
                std::string currensm=config.substr(startPos+1,endPos-1-startPos);
                currensm.erase(std::remove(currensm.begin(), currensm.end(), '"'), currensm.end());
                std::string qtybefore=config.substr(startPos0+1,endPos0-1-startPos0);
                qtybefore.erase(std::remove(qtybefore.begin(), qtybefore.end(), '"'), qtybefore.end());
                int qty=stoi(qtybefore);
                std::string indzabef=config.substr(startPos1+1,endPos1-1-startPos1);
                indzabef.erase(std::remove(indzabef.begin(), indzabef.end(), '"'), indzabef.end());
                int indza=stoi(indzabef);
                std::string kbbefore=config.substr(startPos2+1,endPos2-1-startPos2);
                kbbefore.erase(std::remove(kbbefore.begin(), kbbefore.end(), '"'), kbbefore.end());
                int sizeKb=stoi(kbbefore);
                config=config.substr(config.find("identifier")+12);
                std::string ident=config;
                config=config.substr(0,config.find("<|>}"));
                if(first==true&&qtyAfter==0){
                    remove(kmp.str().c_str());
                    if(currensm==currsm&&qty==stoi(ind)){
                        if(qty>0){
                            qty=qty-1;
                            indza=10;
                        }else{
                            indza=0;
                        }
                    }
                }else{
                    if(currensm==currsm&&qty==stoi(ind)){
                        indza=indza-1;
                    }
                    std::ifstream cont(kmp.str());
                    std::stringstream *buffer = new std::stringstream;
                    (*buffer) << cont.rdbuf();
                    cont.close();
                    std::string contg = (*buffer).str();
                    buffer->str(std::string());
                    bool exist=false;
                    while(true){
                        if(config.substr(0,config.find("<!>"))==""){
                            break;
                        }
                        std::stringstream kf;
                        kf<<config.substr(0,config.find("<!>"))<<":^:"<<argv[3];
                        std::string conttmp=contg;
                        while(true){
                            if(conttmp.find(kf.str())==std::string::npos){
                                break;
                            }
                            if(conttmp.substr(conttmp.find(kf.str())-3).substr(0,3)!="<|>"&&conttmp.substr(conttmp.find(kf.str())-3).substr(0,3)!=">|<"){
                                conttmp=conttmp.substr(conttmp.find(kf.str()));
                                conttmp=conttmp.substr(conttmp.find("<|>"));
                                continue;
                            }
                            conttmp=conttmp.substr(conttmp.find(kf.str()));
                            if(conttmp.substr(kf.str().size()).substr(0,3)!="<|>"&&conttmp.substr(kf.str().size()).substr(0,3)!=">|<"){
                                conttmp=conttmp.substr(conttmp.find("<|>"));
                                continue;
                            }
                            exist=true;
                            break;
                        }
                        if(exist){
                            int sz=0;
                            int point=0;
                            std::vector<char> seqn={'>','|','<'};
                            size_t cn=contg.find(conttmp);
                            std::string prmt=contg;
                            while(true){
                                if(seqn[point]==prmt.substr(cn-sz)[0]){
                                    point=point+1;
                                    if(point==3){
                                        prmt=prmt.substr(cn-sz);
                                        break;
                                    }
                                }else{
                                    point=0;
                                }
                                sz=sz+1;
                            }
                            std::stringstream vc;
                            vc<<contg.substr(0,contg.find(prmt))<<conttmp.substr(conttmp.find("<|>"));
                            std::ofstream out(kmp.str());
                            out << vc.str();
                            out.close();
                            break;
                        }
                        config=config.substr(config.find("<!>")+3);
                    }
                }
                std::ofstream outputFile("bound/db/tables/"+tableName+"/find.bd");
                outputFile << findNw.str();
                outputFile.close();
                std::stringstream lst;
                lst<<"{\"currensm\":\""<<currensm<<"\"<|>\"qty\":\""<<qty<<"\"<|>\"indza\":\""<<indza<<"\"<|>\"sizeKb\":\""<<sizeKb<<"\"<|>\"identifier\":"<<ident;
                std::ofstream out0("bound/db/tables/"+tableName+"/config.bd");
                out0 << lst.str();
                out0.close();
                remove("bound/db/tables/lock.bd");
                return 0;
            }
        }
    }else {
        std::cout << "Unknown command: " << command << std::endl;
    }

    return 0;
}

