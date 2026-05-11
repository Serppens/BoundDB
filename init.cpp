/*
    Source Code        BoundDB
    Developed by       Serpens
    
    Copyright (c) 2023-2026
    

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
        if(!std::filesystem::exists("/media/bound/db/tables/lock.bd")){
            std::ofstream outputFile("/media/bound/db/tables/lock.bd");
            outputFile << "";
            outputFile.close();
            break;
        }
        sleep(0.5);
    }
    return 0;
}
std::string openSafe(std::string pth){
    if(std::filesystem::exists(pth)){
        while(true){
            std::ifstream conf(pth);
            if (!conf) {
                sleep(0.5);
                continue;
            }
            std::ostringstream ss;
            ss << conf.rdbuf();
            return ss.str();
        }
    }
    return "nothing";
}
std::string insertSafe(std::string pth,std::string tb){
    std::string str=openSafe(pth);
    if(str=="nothing"){
        return "n";
    }
    if(str.length()<100){
        int q_to=100-str.length();
        int pos=q_to;
        std::string nw="";
        for(int i=0;i<q_to;i++){
            nw+=" ";
        }
        str=str+nw;
    }
    int cnt=0;
    std::string z=">>$";
    for(int i=0;i<str.length();i++){
        z+=str[i];
        cnt+=1;
        if(cnt==10){
            z+=">>$";
            cnt=0;
        }
    }
    return z;
    /*
        Acordei, um dia quente e ensolarado sem nuvens, aparentemente não choverá hoje, provavelmente amanhã fará mais calor, enquanto o dia passa parece que settei um loop, espero que esse seja somente um for curto e não um while true, lol
    */
}
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: /media/FileDash/bound <command>" << std::endl;
        return 1;
    }
    std::string command = argv[1];
    if(command=="help"){
        std::cout<<"Commands: \n    create_table\n      /media/FileDash/bound create_table <NameTable> <[identifier1,identifier2...]>\n      "<<std::endl;
    }else if(command=="create_table"){
        if(argc<4){
            std::cout << "Usage: /media/FileDash/bound create_table <NameTable> <[name_identifier0...]>" << std::endl;
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
        if(!std::filesystem::exists("/media/bound/db/tables/")){
            std::filesystem::create_directory("/media/bound/db/tables/");
        }
        if(std::filesystem::exists("/media/bound/db/tables/"+nameTable+"/config.bd")){
            std::cout << "This table already exists"<<std::endl;
            return 1;
        }
        std::filesystem::create_directory("/media/bound/db/tables/"+nameTable);

        std::stringstream nw;
        nw<<"{\"currensm\":\"md201\"<|>\"qty\":\"0\"<|>\"indza\":\"0\"<|>\"sizeKb\":\"0\"<|>\"identifier\":";
        nw<<wlt.str();
        nw<<"<|>}";
        std::ofstream outputFile("/media/bound/db/tables/"+nameTable+"/config.bd");
        outputFile << nw.str();
        outputFile.close();
        remove("/media/bound/db/tables/lock.bd");
        return 0;
    }
    else if (command=="insert") {
        if(argc<3){
            std::cout << "Usage: /media/FileDash/bound insert table_name [identifier=value,column0=value...]" << std::endl;
            return -1;
        }
        std::string tableName=argv[2];
        if(!std::filesystem::exists("/media/bound/db/tables/"+tableName)){
            std::cout << "This table not exist" << std::endl;
            return -1;
        }
        if(std::filesystem::exists("/media/bound/db/tables/"+tableName+"/insert.bd")){
            lock();
            std::string arg=openSafe("/media/bound/db/tables/"+tableName+"/insert.bd");
            std::string currensm;
            int qty;
            int currentBd;
            int indza;
            int sizeKb;
            std::string fileContent = openSafe("/media/bound/db/tables/"+tableName+"/config.bd");
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
                std::string wis="/media/bound/db/tables/"+tableName+"/insert.bd";
                remove(wis.c_str());
                remove("/media/bound/db/tables/lock.bd");
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
                        std::string directoryPath = "/media/bound/db/tables/"+tableName+"/"+generatedPassword;
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
            if(!std::filesystem::exists("/media/bound/db/tables/"+tableName+"/"+currensm)){
                ss<<"<|>";
                std::filesystem::create_directory("/media/bound/db/tables/"+tableName+"/"+currensm);
            }else if(std::filesystem::exists("/media/bound/db/tables/"+tableName+"/"+currensm+"/"+std::to_string(qty)+".bd")){
                std::string cnt=openSafe("/media/bound/db/tables/"+tableName+"/"+currensm+"/"+std::to_string(qty)+".bd");
                ss<<cnt;
            }else{
                ss<<"<|>";
            }
            std::string cont;
            if(std::filesystem::exists("/media/bound/db/tables/"+tableName+"/find.bd")){
                cont = openSafe("/media/bound/db/tables/"+tableName+"/find.bd");
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
                    std::cout << "Usage: /media/FileDash/bound insert table_name [column0:^:value^|^column1:^:value...]0" << std::endl;
                    std::string wis="/media/bound/db/tables/"+tableName+"/insert.bd";
                    remove(wis.c_str());
                    remove("/media/bound/db/tables/lock.bd");
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
                                    std::string wis="/media/bound/db/tables/"+tableName+"/insert.bd";
                                    remove(wis.c_str());
                                    remove("/media/bound/db/tables/lock.bd");
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
            std::ofstream xzx("/media/bound/db/tables/"+tableName+"/find.bd");
            xzx<<s.str();
            xzx.close();
            std::stringstream xnw;
            xnw<<"{\"currensm\":\""+currensm+"\"<|>\"qty\":\""+std::to_string(qty)+"\"<|>\"indza\":\""+std::to_string(indza)+"\"<|>\"sizeKb\":\"0\"<|>\"identifier\":";
            xnw<<cld;
            xnw<<"<|>}";
            std::ofstream xcx("/media/bound/db/tables/"+tableName+"/config.bd");
            xcx<<xnw.str();
            xcx.close();
            std::ofstream outputFile("/media/bound/db/tables/"+tableName+"/"+currensm+"/"+std::to_string(qty)+".bd");
            outputFile << ss.str();
            outputFile.close();
            std::string wis="/media/bound/db/tables/"+tableName+"/insert.bd";
            remove(wis.c_str());
            remove("/media/bound/db/tables/lock.bd");
            return 0;
        }
    }else if(command=="findMultiple"){
        if(argc<4){
            std::cout << "Usage: /media/FileDash/bound findMultiple table [identification0,identification1...]" << std::endl;
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
        if(!std::filesystem::exists("/media/bound/db/tables/"+tableName+"/find.bd")){
            std::cout<<"This identification not exists"<<std::endl;
            return 1;
        }
        lock();
        std::string prty = openSafe("/media/bound/db/tables/"+tableName+"/find.bd");
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
                            std::string invr = "/media/bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd";
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
                            remove("/media/bound/db/tables/lock.bd");
                            return -1;
                        }
                    }
                }
            }
        }
        std::cout<<hllwd<<std::endl;
        remove("/media/bound/db/tables/lock.bd");
        return -1;
    }else if(command=="find"){
        if(argc<4){
            std::cout << "Usage: /media/FileDash/bound find table identification" << std::endl;
            return 1;
        }
        std::string tableName=argv[2];
        if(!std::filesystem::exists("/media/bound/db/tables/"+tableName+"/find.bd")){
            std::cout<<"This identification not exists"<<std::endl;
            return 1;
        }
        lock();
        std::string prty = openSafe("/media/bound/db/tables/"+tableName+"/find.bd");
        std::stringstream ar;
        ar<<argv[3];
        ar<<"<!>";
        size_t nt=prty.find(ar.str());
        if(nt==std::string::npos){
            std::cout<<"This identification not exists"<<std::endl;
            remove("/media/bound/db/tables/lock.bd");
            return -1;
        }else{
            std::string conttmp=prty;
            while(true){
                size_t cn=conttmp.find(ar.str());
                if(cn==std::string::npos){
                    std::cout<<"This identification not exists"<<std::endl;
                    remove("/media/bound/db/tables/lock.bd");
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
                std::string invr = openSafe("/media/bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
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
                remove("/media/bound/db/tables/lock.bd");
                return 0;
            }
            std::cout<<"This identification not exists"<<std::endl;
            remove("/media/bound/db/tables/lock.bd");
            return -1;
        }
    }else if(command=="add_param_array"){
        if(argc!=6){
            std::cout << "Usage: bound add_param_array <NameTable> <Identification> <name_column_array,sub_name_column_array,sub_sub_name_column_array...> <uuid_base | -1>" << std::endl;
            return -1;
        }
        std::string tableName=argv[2];
        if(std::filesystem::exists("/media/bound/db/tables/"+tableName+"/add_param_array.bd")){
            std::string tb=argv[3];
            if(tableName.find("<|>")!=std::string::npos||tableName.find(">|<")!=std::string::npos||tableName.find("<!>")!=std::string::npos||
            tb.find("<|>")!=std::string::npos||tb.find(">|<")!=std::string::npos||tb.find("<!>")!=std::string::npos){
                std::string wis="/media/bound/db/tables/"+tableName+"/add_param_array.bd";
                remove(wis.c_str());
                return -1;
            }
            if(!std::filesystem::exists("/media/bound/db/tables/"+tableName)){
                std::cout << "This table not exist" << std::endl;
                std::string wis="/media/bound/db/tables/"+tableName+"/add_param_array.bd";
                remove(wis.c_str());
                return -1;
            }
            lock();
            std::string prty = openSafe("/media/bound/db/tables/"+tableName+"/find.bd");
            std::stringstream ar;
            ar<<argv[3];
            ar<<"<!>";
            size_t nt=prty.find(ar.str());
            if(nt==std::string::npos){
                std::cout<<"This identification not exists"<<std::endl;
                remove("/media/bound/db/tables/lock.bd");
                std::string wis="/media/bound/db/tables/"+tableName+"/add_param_array.bd";
                remove(wis.c_str());
                return -1;
            }else{
                std::string conttmp=prty;
                while(true){
                    size_t cn=conttmp.find(ar.str());
                    if(cn==std::string::npos){
                        std::cout<<"This identification not exists"<<std::endl;
                        remove("/media/bound/db/tables/lock.bd");
                        std::string wis="/media/bound/db/tables/"+tableName+"/add_param_array.bd";
                        remove(wis.c_str());
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
                    std::string invr = openSafe("/media/bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                    std::stringstream bfr;
                    bfr<<invr.substr(0,invr.find("<|>")+3);
                    invr=invr.substr(invr.find("<|>")+3);
                    if(stoi(ind)!=0){
                        for(int i=0;i<stoi(ind);i++){
                            bfr<<invr.substr(0,invr.find("<|>")+3);
                            invr=invr.substr(invr.find("<|>")+3);
                        }
                    }
                    std::string nvvc = openSafe("/media/bound/db/tables/"+tableName+"/add_param_array.bd");
                    size_t rs=nvvc.find(":^:");
                    if(rs==std::string::npos){
                        std::cout<<"Your parameters can't be empty"<<std::endl;
                        remove("/media/bound/db/tables/lock.bd");
                        std::string wis="/media/bound/db/tables/"+tableName+"/add_param_array.bd";
                        remove(wis.c_str());
                        return -1;
                    }
                    std::stringstream xs;
                    std::string afr=invr.substr(invr.find("<|>"));
                    invr=invr.substr(0,invr.find("<|>"));
                    std::string uuid=get_uuid();
                    /*
                        1. we need modify everything to work as conversor, remember after use !#! no need from separator to another parameter example:
                            key_parameter!@!:^:value_parameter^<>!#!>|<key_column:^:value_column -> Wrong
                            key_parameter!@!:^:value_parameter^<>!#!key_column:^:value_column -> Right
                            Obs: it need from a fix, is more simple keep >|< after !#! as right
                        
                        2. Values is: 
                            ^<> = new dict/obj in array or delimit end of the array, it's obrigatory when have some dict/obj to identify end eg:
                                key_parameter!@!key_parameter:^:value_parameter!#! -> Wrong
                                key_parameter!@!key_parameter:^:value_parameter^<>!#! -> Right
                            >|< = it's a , inside the same obj/dict using it reset to another eg:
                                array!@!key_parameter:^:value_parameter>|<another_key:^:another_value^<>!#!
                                    result: array=[{"key_parameter":"value_parameter","another_key":"another_value"}]
                   
                    */
                    std::string tl=argv[4];
                    std::vector<std::string>arx=parse_dict(tl);
                    std::vector<bool>arxexs;
                    for(int i=0;i<arx.size();i++){
                        arxexs.push_back(false);
                    }
                    int arxpnt=0;
                    std::string xis="/media/bound/db/tables/"+tableName+"/add_param_array.bd";
                    std::string invrbfr="";
                    std::string invrafr="";
                    std::vector<char>prm0={'!','@','!'};
                    int prm0pnt=0;
                    std::vector<char>prm1={'!','#','!'};
                    int prm1pnt=0;
                    std::vector<char>prmdc0={'>','|','<'};
                    int prmdc0pnt=0;
                    std::vector<char>prmdc1={'{','>','}'};
                    int prmdc1pnt=0;
                    std::vector<char>prmdc2={'{','<','}'};
                    int prmdc2pnt=0;
                    std::vector<char>prmdc3={'^','<','>'};
                    int prmdc3pnt=0;
                    int posPrm=0;
                    int cix=0;
                    int stpdc2=0;
                    int stparx=0;
                    int cix_=0;
                    int cixone=0;
                    bool skp=false;
                    /*
                        cix_ variable
                         current position from array right

                        cix variable
                         0 = nothing array using
                         -1 = using a not correspondent array / skip it
                         -2 = using a add_content_child / skip it
                         -3 = here we are inside correspondent array but it join in a new sub_array not correspondent with currently input then we'll inside in this to skip
                         1 = using a correspondent array
                         2 = using a correspondent array with sub_child but input 'uuid_base' from user is not equal with this content then need skip
                    */
                    int igf=0;
                    std::string pntdefine="";
                    int pos=-1;
                    std::string strr=argv[5];
                    std::vector<char>lco={':','^',':'};
                    int lcopnt=0;
                    int skc=0;
                    std::string skcstr="";
                    bool vtv=false;
                    if(strr!="-1"){
                        for(int i=0;i<invr.size();i++){
                            if(skc==0){
                                if(prmdc0[prmdc0pnt]==invr[i]){
                                    prmdc0pnt+=1;
                                }else{
                                    prmdc0pnt=0;
                                    if(prmdc0[prmdc0pnt]==invr[i]){
                                        prmdc0pnt+=1;
                                    }
                                }
                                if(prmdc0pnt==3){
                                    skc=1;
                                    prmdc0pnt=0;
                                }
                            }else if(skc==1){
                                if(lco[lcopnt]==invr[i]){
                                    lcopnt+=1;
                                }else{
                                    lcopnt=0;
                                    if(lco[lcopnt]==invr[i]){
                                        lcopnt+=1;
                                    }
                                }
                                if(lcopnt==3){
                                    skcstr=skcstr.substr(0,skcstr.size()-2);
                                    std::cout<<skcstr<<std::endl;
                                    if(skcstr=="uuid_base"){
                                        skc=2;
                                        skcstr="";
                                        lcopnt=0;
                                        continue;
                                    }
                                    skc=0;
                                    skcstr="";
                                    lcopnt=0;
                                    continue;
                                }
                                skcstr+=invr[i];
                            }else{
                                if(prmdc0[prmdc0pnt]==invr[i]){
                                    prmdc0pnt+=1;
                                }else{
                                    prmdc0pnt=0;
                                    if(prmdc0[prmdc0pnt]==invr[i]){
                                        prmdc0pnt+=1;
                                    }
                                }
                                if(prmdc0pnt==3){
                                    skcstr=skcstr.substr(0,skcstr.size()-2);
                                    if(skcstr==strr){
                                        /*    
                                            std::vector<char>prm0={'!','@','!'};
                                            int prm0pnt=0;
                                            std::vector<char>prm1={'!','#','!'};
                                            int prm1pnt=0;
                                        */
                                        vtv=true;
                                        std::vector<char>prmdc3tmp={'>','<','^'};
                                        prm0pnt=0;
                                        prmdc3pnt=0;
                                        std::string brs="";
                                        int vj=0;
                                        prm1pnt=0;
                                        prm0pnt=0;
                                        std::cout<<invr<<std::endl;
                                        for(int xy=i;xy<invr.size();xy--){
                                            if(prm1[prm1pnt]==invr[xy]){
                                                prm1pnt+=1;
                                            }else{
                                                prm1pnt=0;
                                                if(prm1[prm1pnt]==invr[xy]){
                                                    prm1pnt+=1;
                                                }   
                                            }
                                            if(prm1pnt==3){
                                                prm0pnt=0;
                                                prm1pnt=0;
                                                prmdc3pnt=0;
                                            }
                                            if(prm0[prm0pnt]==invr[xy]){
                                                prm0pnt+=1;
                                            }else{
                                                prm0pnt=0;
                                                if(prm0[prm0pnt]==invr[xy]){
                                                    prm0pnt+=1;
                                                }
                                            }
                                            if(prmdc3tmp[prmdc3pnt]==invr[xy]){
                                                prmdc3pnt+=1;
                                            }else{
                                                prmdc3pnt=0;
                                                if(prmdc3tmp[prmdc3pnt]==invr[xy]){
                                                    prmdc3pnt+=1;
                                                }
                                            }
                                            brs+=invr[xy];
                                            if(prm0pnt==3||prmdc3pnt==3){
                                                prm1pnt=0;
                                                prm0pnt=0;
                                                vj=xy;
                                                vj+=3;
                                                break;
                                            }
                                        }
                                        prmdc3pnt=0;
                                        prm0pnt=0;
                                        i++;
                                        int paf=0;
                                        std::string btd="";
                                        int ard=0;
                                        int stn=0;
                                        prm1pnt=0;
                                        
                                        for(int x=i;x<invr.size();x++){
                                            btd+=invr[x];
                                            if(ard==0){
                                                if(prm0[prm0pnt]==invr[x]){
                                                    prm0pnt+=1;
                                                }else{
                                                    prm0pnt=0;
                                                    if(prm0[prm0pnt]==invr[x]){
                                                        prm0pnt+=1;
                                                    }   
                                                }
                                                if(prm0pnt==3){
                                                    prm0pnt=0;
                                                    prmdc3pnt=0;
                                                    stn+=1;
                                                    ard=1;
                                                }

                                                if(prmdc3[prmdc3pnt]==invr[x]){
                                                    prmdc3pnt+=1;
                                                }else{
                                                    prmdc3pnt=0;
                                                    if(prmdc3[prmdc3pnt]==invr[x]){
                                                        prmdc3pnt+=1;
                                                    }   
                                                }
                                                if(prmdc3pnt==3){
                                                    prm0pnt=0;
                                                    paf=x-2;
                                                    break;
                                                }
                                            }else{
                                                if(prm0[prm0pnt]==invr[x]){
                                                    prm0pnt+=1;
                                                }else{
                                                    prm0pnt=0;
                                                    if(prm0[prm0pnt]==invr[x]){
                                                        prm0pnt+=1;
                                                    }   
                                                }
                                                if(prm0pnt==3){
                                                    prm0pnt=0;
                                                    prm1pnt=0;
                                                    stn+=1;
                                                }
                                                if(prm1[prm1pnt]==invr[x]){
                                                    prm1pnt+=1;
                                                }else{
                                                    prm1pnt=0;
                                                    if(prm1[prm1pnt]==invr[x]){
                                                        prm1pnt+=1;
                                                    }   
                                                }
                                                if(prm1pnt==3){
                                                    prm0pnt=0;
                                                    prm1pnt=0;
                                                    stn-=1;
                                                }
                                                if(stn==0){
                                                    prm0pnt=0;
                                                    prm1pnt=0;
                                                    ard=0;
                                                }
                                            }
                                        }
                                        std::reverse(brs.begin(),brs.end());
                                        invrbfr=invr.substr(0,vj);
                                        invrafr=invr.substr(paf);
                                        invr=brs.substr(3,brs.size())+btd.substr(0,btd.size()-3);
                                    }
                                    skcstr="";
                                    skc=0;
                                    prmdc0pnt=0;
                                    continue;
                                }
                                skcstr+=invr[i];
                            }
                            continue;
                        }
                    }
                    std::cout<<invr<<std::endl;
                    for(int i=0;i<invr.size();i++){
                        if(cix==-2){
                            if(invr[i]==prmdc1[prmdc1pnt]){
                                prmdc1pnt+=1;
                            }else{
                                prmdc1pnt=0;
                                if(invr[i]==prmdc1[prmdc1pnt]){
                                    prmdc1pnt+=1;
                                } 
                            }
                            if(prmdc1pnt==3){
                                prmdc1pnt=0;
                                prmdc2pnt=0;
                                stpdc2+=1;
                            }
                            if(invr[i]==prmdc2[prmdc2pnt]){
                                prmdc2pnt+=1;
                            }else{
                                prmdc2pnt=0;
                                if(invr[i]==prmdc2[prmdc2pnt]){
                                    prmdc2pnt+=1;
                                } 
                            }
                            if(prmdc2pnt==3){
                                prmdc1pnt=0;
                                prmdc2pnt=0;
                                stpdc2-=1;
                            }
                            if(stpdc2==0){
                                cix=0;
                                continue;
                            }
                        }else if(cix==-1){
                            if(invr[i]==prm0[prm0pnt]){
                                prm0pnt+=1;
                            }else{
                                prm0pnt=0;
                                if(invr[i]==prm0[prm0pnt]){
                                    prm0pnt+=1;
                                } 
                            }
                            if(prm0pnt==3){
                                prm0pnt=0;
                                prm1pnt=0;
                                stpdc2+=1;
                            }
                            if(invr[i]==prm1[prm1pnt]){
                                prm1pnt+=1;
                            }else{
                                prm1pnt=0;
                                if(invr[i]==prm1[prm1pnt]){
                                    prm1pnt+=1;
                                } 
                            }
                            if(prm1pnt==3){
                                prm0pnt=0;
                                prm1pnt=0;
                                stpdc2-=1;
                            }
                            if(stpdc2==0){
                                cix=0;
                                continue;
                            }
                        }else if(cix==-3){
                            if(invr[i]==prm0[prm0pnt]){
                                prm0pnt+=1;
                            }else{
                                prm0pnt=0;
                                if(invr[i]==prm0[prm0pnt]){
                                    prm0pnt+=1;
                                } 
                            }
                            if(prm0pnt==3){
                                prm0pnt=0;
                                prm1pnt=0;
                                stparx+=1;
                            }
                            if(invr[i]==prm1[prm1pnt]){
                                prm1pnt+=1;
                            }else{
                                prm1pnt=0;
                                if(invr[i]==prm1[prm1pnt]){
                                    prm1pnt+=1;
                                } 
                            }
                            if(prm1pnt==3){
                                prm0pnt=0;
                                prm1pnt=0;
                                stparx-=1;
                            }
                            if(stparx==0){
                                cix=1;
                                continue;
                            }
                        }else if(cix==1){
                            if(invr[i]==prmdc0[prmdc0pnt]){
                                prmdc0pnt+=1;
                            }else{
                                prmdc0pnt=0;
                                if(invr[i]==prmdc0[prmdc0pnt]){
                                    prmdc0pnt+=1;
                                } 
                            }
                            if(prmdc0pnt==3){
                                prmdc0pnt=0;
                                prmdc3pnt=0;
                                prm0pnt=0;
                                prm1pnt=0;
                                pntdefine="";
                                continue;
                            }
                            if(invr[i]==prmdc3[prmdc3pnt]){
                                prmdc3pnt+=1;
                            }else{
                                prmdc3pnt=0;
                                if(invr[i]==prmdc3[prmdc3pnt]){
                                    prmdc3pnt+=1;
                                }
                            }
                            if(prmdc3pnt==3){
                                if(cix_==1&&stpdc2<2){
                                    cixone+=1;
                                }
                                prmdc0pnt=0;
                                prmdc3pnt=0;
                                prm0pnt=0;
                                prm1pnt=0;
                                pntdefine="";
                                continue;
                            }
                            // if cix_==0 continue finding array right
                            if(prm0[prm0pnt]==invr[i]){
                                prm0pnt+=1;
                            }else{
                                prm0pnt=0;
                                if(prm0[prm0pnt]==invr[i]){
                                    prm0pnt+=1;
                                }
                            }
                            if(prm0pnt==3){
                                bool insjoin=false;
                                int dctmp=0;
                                bool gtv=false;
                                std::vector<char>prmdc0tmp={'<','|','>'};
                                std::vector<char>prmdc3={'^','<','>'};
                                std::vector<char>prmdc3tmp={'>','<','^'};
                                std::string nervs="";
                                std::string pntdefinex="";
                                std::string pntdefineb="";
                                prm0pnt=0;
                                prm1pnt=0;
                                prmdc0pnt=0;
                                prmdc3pnt=0;
                                bool insc=false;
                                for(int xl=i-3;xl<invr.size();xl--){
                                    nervs+=invr[xl];
                                    if(insjoin){
                                        if(invr[xl]==prm1[prm1pnt]){
                                            prm1pnt+=1;
                                        }else{
                                            prm1pnt=0;
                                            if(invr[xl]==prm1[prm1pnt]){
                                                prm1pnt+=1;
                                            }
                                        }
                                        if(prm1pnt==3){
                                            prm0pnt=0;
                                            prm1pnt=0;
                                            dctmp+=1;
                                        }
                                        if(invr[xl]==prm0[prm0pnt]){
                                            prm0pnt+=1;
                                        }else{
                                            prm0pnt=0;
                                            if(invr[xl]==prm0[prm0pnt]){
                                                prm0pnt+=1;
                                            }
                                        }
                                        if(prm0pnt==3){
                                            prm0pnt=0;
                                            prm1pnt=0;
                                            dctmp-=1;
                                        }
                                        if(dctmp==0){
                                            insjoin=false;
                                        }
                                    }else{
                                        if(gtv){
                                            if(prmdc0tmp[prmdc0pnt]==invr[xl]){
                                                prmdc0pnt+=1;
                                            }else{
                                                prmdc0pnt=0;
                                                if(prmdc0tmp[prmdc0pnt]==invr[xl]){
                                                    prmdc0pnt+=1;
                                                }
                                            }
                                            if(prmdc0pnt==3){
                                                prmdc0pnt=0;
                                                gtv=false;
                                                std::reverse(pntdefineb.begin(),pntdefineb.end());
                                                std::reverse(pntdefinex.begin(),pntdefinex.end());
                                                if(pntdefinex=="|<uuid_base"){
                                                    if(strr!="-1"&&pntdefineb!="^:"+strr){
                                                        insc=true;
                                                    }
                                                    break;
                                                }
                                                pntdefinex="";
                                                pntdefineb="";
                                                continue;
                                            }
                                            pntdefinex+=invr[xl];
                                            continue;
                                        }
                                        if(lco[lcopnt]==invr[xl]){
                                            lcopnt+=1;
                                        }else{
                                            lcopnt=0;
                                            if(lco[lcopnt]==invr[xl]){
                                                lcopnt+=1;
                                            }
                                        }
                                        if(lcopnt==3){
                                            lcopnt=0;
                                            prm0pnt=0;
                                            prm1pnt=0;
                                            prmdc0pnt=0;
                                            prmdc1pnt=0;
                                            prmdc2pnt=0;
                                            prmdc3pnt=0;
                                            gtv=true;
                                            continue;
                                        }
                                        if(invr[xl]==prm0[prm0pnt]){
                                            prm0pnt+=1;
                                        }else{
                                            prm0pnt=0;
                                            if(invr[xl]==prm0[prm0pnt]){
                                                prm0pnt+=1;
                                            }
                                        }
                                        if(prm0pnt==3){
                                            lcopnt=0;
                                            prm0pnt=0;
                                            prm1pnt=0;
                                            prmdc0pnt=0;
                                            prmdc1pnt=0;
                                            prmdc2pnt=0;
                                            prmdc3pnt=0;
                                            break;
                                        }
                                        if(invr[xl]==prmdc3tmp[prmdc3pnt]){
                                            prmdc3pnt+=1;
                                        }else{
                                            prmdc3pnt=0;
                                            if(invr[xl]==prmdc3tmp[prmdc3pnt]){
                                                prmdc3pnt+=1;
                                            }
                                        }
                                        if(prmdc3pnt==3){
                                            lcopnt=0;
                                            prm0pnt=0;
                                            prm1pnt=0;
                                            prmdc0pnt=0;
                                            prmdc1pnt=0;
                                            prmdc2pnt=0;
                                            prmdc3pnt=0;
                                            break;
                                        }
                                        if(invr[xl]==prm1[prm1pnt]){
                                            prm1pnt+=1;
                                        }else{
                                            prm1pnt=0;
                                            if(invr[xl]==prm1[prm1pnt]){
                                                prm1pnt+=1;
                                            }
                                        }
                                        if(prm1pnt==3){
                                            dctmp=1;
                                            insjoin=true;
                                            lcopnt=0;
                                            prm0pnt=0;
                                            prm1pnt=0;
                                            prmdc0pnt=0;
                                            prmdc1pnt=0;
                                            prmdc2pnt=0;
                                            prmdc3pnt=0;
                                        }
                                        pntdefineb+=invr[xl];
                                    }
                                }
                                std::reverse(nervs.begin(),nervs.end());
                                if(cix_==0){
                                    pntdefine=pntdefine.substr(0,pntdefine.size()-2);
                                    if(pntdefine==arx[arxpnt]&&!insc){
                                        arxexs[arxpnt]=true;
                                        stpdc2=1;
                                        arxpnt+=1;
                                        posPrm+=1;
                                        if(arx.size()==posPrm){
                                            cix_=1;
                                        }
                                        pntdefine="";
                                    }else{
                                        /*aqui precisamos ver se o input do usuário especifica um uuid_base se especificar então terá de achar esse para adicionar um parametro novo*/
                                        stparx=1;
                                        pntdefine="";
                                        cix=-3;
                                    }
                                    continue;
                                }else{
                                    //aqui se der algum erro é um ponto a ser visto
                                    stpdc2+=1;
                                }
                            }
                            if(prm1[prm1pnt]==invr[i]){
                                prm1pnt+=1;
                            }else{
                                prm1pnt=0;
                                if(prm1[prm1pnt]==invr[i]){
                                    prm1pnt+=1;
                                }
                            }
                            if(prm1pnt==3){
                                prm0pnt=0;
                                prm1pnt=0;
                                stpdc2-=1;
                            }
                            if(stpdc2==0){
                                pos=i-2;
                                break;
                            }
                            pntdefine+=invr[i];
                        }else if(cix==2){
                            if(invr[i]==prm0[prm0pnt]){
                                prm0pnt+=1;
                            }else{
                                prm0pnt=0;
                                if(invr[i]==prm0[prm0pnt]){
                                    prm0pnt+=1;
                                } 
                            }
                            if(prm0pnt==3){
                                prm0pnt=0;
                                prm1pnt=0;
                                stparx+=1;
                            }
                            if(invr[i]==prm1[prm1pnt]){
                                prm1pnt+=1;
                            }else{
                                prm1pnt=0;
                                if(invr[i]==prm1[prm1pnt]){
                                    prm1pnt+=1;
                                } 
                            }
                            if(prm1pnt==3){
                                prm0pnt=0;
                                prm1pnt=0;
                                stparx-=1;
                            }
                            if(stparx==0){
                                cix=0;
                                continue;
                            }
                        }else if(cix==0){
                            if(prm0[prm0pnt]==invr[i]){
                                prm0pnt+=1;
                            }else{
                                prm0pnt=0;
                                if(prm0[prm0pnt]==invr[i]){
                                    prm0pnt+=1;
                                }
                            }
                            if(prm0pnt==3){
                                prm0pnt=0;
                                prm1pnt=0;
                                prmdc0pnt=0;
                                prmdc1pnt=0;
                                prmdc2pnt=0;
                                /*temos que ter outra coisa para checar se de fato ele entrou num array correspondente ou faz parte do uuid_base. Percebemos que o sistema de sub_array tem um erro de lógica
                                    Uma explicação simples do erro de lógica:
                                    Quando o input for array,sub_array caso o sub_array estiver dentro de um contéudo do array ou seja com base/uuid_base ele erroneamente entrará nisso por não verificar se
                                    tratar de um contéudo mesmo que o ultimo indice do input do usuário não tenha explicitamente nada ou esteja apontando
                                    para um uuid_base diferente, portanto aqui a idéia é arrumarmos isso.
                                    Básicamente funcionará da seguinte maneira: 
                                        .   Caso o ultimo indice seja -1 então ele adicionar o sub_array como sendo não parte de um contéudo mas sim um sub_array que faz
                                            parte do array_principal como si mesmo um contéudo
                                        .   Caso esse seja um uuid_base então evidente que será procurado por contéudos de dentro do array específico do input.

                                    Aqui clareando mais existem duas regras no mesmo sistema, regra de contéudo e regra de arrays cada qual tem sua utilidade específica mas
                                    que ambas funcionam juntas e são valiosas para uma consulta rápida e eficiente pois dessa forma evitamos que seja necessário
                                    o usuário apontar várias vezes para lugar diferentes da memória afim de resolver uma consulta complexa
                                
                                    então teremos que ver tudo por exemplo se houver !#! entre o retrocedimento então a gente vai pular ele dentro do próprio check normalmente, 
                                    para sabermos se é ou não contéudo temos que fazer o seguinte, ver se uuid_base vai aparecer antes ou depois da flag ^<> ou se !@! vai aparecer antes ou depos de !#! 
                                    nesse caso se uuid_base aparecer antes de ^<> então está dentro de um contéudo, se !@! aparecer antes de qualquer coisa então dentro de array se não de ambos é op
                                    /
                                */                                
                                pntdefine=pntdefine.substr(0,pntdefine.size()-2);
                                if(pntdefine==arx[arxpnt]){
                                    posPrm=1;
                                    if(arx.size()==posPrm){
                                        cix_=1;
                                        // its the position we need use
                                    }
                                    arxexs[arxpnt]=true;
                                    arxpnt+=1;
                                    stpdc2=1;
                                    cix=1;
                                    pntdefine="";
                                    continue;
                                }
                                stpdc2=1;
                                cix=-1;
                            }
                            if(invr[i]==prmdc0[prmdc0pnt]){
                                prmdc0pnt+=1;
                            }else{
                                prmdc0pnt=0;
                                if(invr[i]==prmdc0[prmdc0pnt]){
                                    prmdc0pnt+=1;
                                }
                            }
                            if(prmdc0pnt==3){
                                pntdefine="";
                                continue;
                            }
                            if(invr[i]==prmdc1[prmdc1pnt]){
                                prmdc1pnt+=1;
                            }else{
                                prmdc1pnt=0;
                                if(invr[i]==prmdc1[prmdc1pnt]){
                                    prmdc1pnt+=1;
                                }
                            }
                            if(prmdc1pnt==3){
                                prm0pnt=0;
                                prm1pnt=0;
                                prmdc0pnt=0;
                                prmdc1pnt=0;
                                prmdc2pnt=0;
                                cix=-2;
                                stpdc2=1;
                                pntdefine="";
                                continue;
                            }
                            pntdefine+=invr[i];
                        }
                    }
                    if(!vtv&&strr!="-1"){
                        std::cout<<"uuid_base not existent"<<std::endl;
                        remove(xis.c_str());
                        remove("/media/bound/db/tables/lock.bd");
                        return -1;
                    }
                    if(pos!=-1){
                        std::string bfr_=invr.substr(0,pos);
                        std::string afr_=invr.substr(pos,invr.size());
                        std::stringstream xvk;
                        nvvc="base:^:"+std::to_string(cixone)+">|<uuid_base:^:"+uuid+">|<"+nvvc;
                        if(arxexs[arxexs.size()-1]==1){
                            xvk<<nvvc<<"^<>";
                        }else{
                            int vtx=0;
                            for(int i=0;i<arxexs.size();i++){
                                if(arxexs[i]==0){
                                    xvk<<arx[i]<<"!@!";
                                    vtx+=1;
                                }
                            }
                            xvk<<nvvc<<"^<>";
                            for(int i=0;i<vtx;i++){
                                xvk<<"!#!";
                            }
                        }
                        std::string endnw;
                        if(strr!="-1"){
                            endnw=invrbfr+bfr_+xvk.str()+afr_+invrafr;
                        }else{
                            endnw=bfr_+xvk.str()+afr_;
                        }
                        std::string out=bfr.str()+endnw+afr;
                        std::ofstream outputFile("/media/bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                        outputFile << out;
                        outputFile.close();
                    }else{
                        invr+=">|<";
                        std::stringstream mxo;
                        for(int i=0;i<arx.size();i++){
                            mxo<<arx[i]<<"!@!";
                            if(i+1==arx.size()){
                                nvvc="base:^:0>|<uuid_base:^:"+uuid+">|<"+nvvc;
                                mxo<<nvvc<<"^<>";
                            }
                        }
                        for(int i=0;i<arx.size();i++){
                            mxo<<"!#!";
                        }
                        invr+=mxo.str();
                        std::ofstream outputFile("/media/bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                        outputFile<<bfr.str();
                        if(strr!="-1"){
                            outputFile<<invrbfr<<invr<<invrafr;
                        }else{
                            outputFile<<invr;
                        }
                        outputFile<<afr;
                        outputFile.close();
                    }
                    remove(xis.c_str());
                    remove("/media/bound/db/tables/lock.bd");
                    return 0;
                }
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
        if(!std::filesystem::exists("/media/bound/db/tables/"+tableName)){
            std::cout << "This table not exist" << std::endl;
            return -1;
        }
        lock();
        
        std::string prty = openSafe("/media/bound/db/tables/"+tableName+"/find.bd");
        std::stringstream ar;
        ar<<argv[3];
        ar<<"<!>";
        size_t nt=prty.find(ar.str());
        if(nt==std::string::npos){
            std::cout<<"This identification not exists"<<std::endl;
            remove("/media/bound/db/tables/lock.bd");
            return -1;
        }else{
            std::string conttmp=prty;
            while(true){
                size_t cn=conttmp.find(ar.str());
                if(cn==std::string::npos){
                    std::cout<<"This identification not exists"<<std::endl;
                    remove("/media/bound/db/tables/lock.bd");
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
                std::string invr = openSafe("/media/bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
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
                                    remove("/media/bound/db/tables/lock.bd");
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
                std::ofstream outputFile("/media/bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                outputFile << bfr.str()<<vcn.str()<<afr;
                outputFile.close();
                remove("/media/bound/db/tables/lock.bd");
                return 1;
            }
        }
    }else if(command=="add_content_child"){
        if(argc!=5){
            std::cout << "Usage: /media/FileDash/bound add_content_child <NameTable> <Identification> <name_column_array0,name_column_array1...> <user:^:DarkSouls_23>|<comment:^:Hello>" << std::endl;
            return -1;
        }
        std::string tableName=argv[2];
        if(std::filesystem::exists("/media/bound/db/tables/"+tableName+"/add_content_child.bd")){
            std::string tb=argv[3];
            if(tableName.find("<|>")!=std::string::npos||tableName.find(">|<")!=std::string::npos||tableName.find("<!>")!=std::string::npos||
            tb.find("<|>")!=std::string::npos||tb.find(">|<")!=std::string::npos||tb.find("<!>")!=std::string::npos){
                std::string wis="/media/bound/db/tables/"+tableName+"/add_content_child.bd";
                remove(wis.c_str());
                return -1;
            }
            if(!std::filesystem::exists("/media/bound/db/tables/"+tableName)){
                std::cout << "This table not exist" << std::endl;
                std::string wis="/media/bound/db/tables/"+tableName+"/add_content_child.bd";
                remove(wis.c_str());
                return -1;
            }
            lock();
            std::string prty = openSafe("/media/bound/db/tables/"+tableName+"/find.bd");
            std::stringstream ar;
            ar<<argv[3];
            ar<<"<!>";
            size_t nt=prty.find(ar.str());
            if(nt==std::string::npos){
                std::cout<<"This identification not exists"<<std::endl;
                remove("/media/bound/db/tables/lock.bd");
                std::string wis="/media/bound/db/tables/"+tableName+"/add_content_child.bd";
                remove(wis.c_str());
                return -1;
            }else{
                std::string conttmp=prty;
                while(true){
                    size_t cn=conttmp.find(ar.str());
                    if(cn==std::string::npos){
                        std::cout<<"This identification not exists"<<std::endl;
                        remove("/media/bound/db/tables/lock.bd");
                        std::string wis="/media/bound/db/tables/"+tableName+"/add_content_child.bd";
                        remove(wis.c_str());
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
                    std::string invr = openSafe("/media/bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
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
                                remove("/media/bound/db/tables/lock.bd");
                                std::string wis="/media/bound/db/tables/"+tableName+"/add_content_child.bd";
                                remove(wis.c_str());
                                return -1;
                            }
                            break;
                        }
                    }
                    rlp=rlp.substr(0,rlp.size()-3);
                    std::string am=openSafe("/media/bound/db/tables/"+tableName+"/add_content_child.bd");
                    if(rlp==""){
                        if(!std::filesystem::exists("/media/bound/db/tables/"+tableName+"/yxy__xyx")){
                            std::filesystem::create_directory("/media/bound/db/tables/"+tableName+"/yxy__xyx");
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
                            if (!std::filesystem::exists("/media/bound/db/tables/"+tableName+"/yxy__xyx/"+generatedPassword+".bd")) {
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
                        std::ofstream outputFile("/media/bound/db/tables/"+tableName+"/yxy__xyx/"+xsx+".bd");
                        outputFile << "{>}"+am;
                        outputFile.close();
                        std::string vvv;
                        std::string end=bfrl+rlp;
                        std::string nw_uuid=get_uuid();
                        nsn+="\""+nw_uuid+"\"]";
                        vvv=bfrl+"y_n:^:y>|<uuid_base:^:"+nw_uuid+">|<pth:^:"+xsx+"^<>"+invr.substr(end.size());
                        std::ofstream xxs("/media/bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                        xxs << bfr.str()<<vvv<<afr;
                        xxs.close();
                        remove("/media/bound/db/tables/lock.bd");
                        std::string wis="/media/bound/db/tables/"+tableName+"/add_content_child.bd";
                        remove(wis.c_str());
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
                        remove("/media/bound/db/tables/lock.bd");
                        std::string wis="/media/bound/db/tables/"+tableName+"/add_content_child.bd";
                        remove(wis.c_str());
                        return -1;
                    }else if(rlp.find("pth")!=std::string::npos){
                        rlp=rlp.substr(rlp.find("pth")+6);
                        rlp=rlp.substr(0,rlp.size()-3);
                        if(am!=""){
                            nsn=nsn.substr(0,nsn.size()-1);
                            nsn+="]";
                            std::string prty = openSafe("/media/bound/db/tables/"+tableName+"/yxy__xyx/"+rlp+".bd");
                            prty=prty.substr(0,prty.size()-3);
                            std::string uuid=get_uuid();
                            prty+="uuid_base:^:"+uuid+">|<"+am+"^<>{<}";
                            std::ofstream xxs("/media/bound/db/tables/"+tableName+"/yxy__xyx/"+rlp+".bd");
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
                        remove("/media/bound/db/tables/lock.bd");
                        std::string wis="/media/bound/db/tables/"+tableName+"/add_content_child.bd";
                        remove(wis.c_str());
                        return 1;
                    }
                    std::cout<<"You can't create a item in this lst"<<std::endl;
                    remove("/media/bound/db/tables/lock.bd");
                    std::string wis="/media/bound/db/tables/"+tableName+"/add_content_child.bd";
                    remove(wis.c_str());
                    return -1;
                }
            }
        }
        return 0;
    }else if(command=="findChild"){
        if(argc!=7){
            std::cout << "Usage: /media/FileDash/bound findChild <NameTable> <Identification> <name_column_array0,name_column_array1...> <specify || all> <if specify: uuid_base elif all: -1>"<<std::endl;
            return -1;
        }
        std::string tableName=argv[2];
        if(!std::filesystem::exists("/media/bound/db/tables/"+tableName)){
            std::cout << "This table not exist" << std::endl;
            return -1;
        }
        lock();
        std::string prty = openSafe("/media/bound/db/tables/"+tableName+"/find.bd");
        std::stringstream ar;
        ar<<argv[3];
        ar<<"<!>";
        size_t nt=prty.find(ar.str());
        if(nt==std::string::npos){
            std::cout<<"This identification not exists"<<std::endl;
            remove("/media/bound/db/tables/lock.bd");
            return 1;
        }else{
            std::string conttmp=prty;
            while(true){
                size_t cn=conttmp.find(ar.str());
                if(cn==std::string::npos){  
                    std::cout<<"This identification not exists"<<std::endl;
                    remove("/media/bound/db/tables/lock.bd");
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
                std::string invr = openSafe("/media/bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
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
                            remove("/media/bound/db/tables/lock.bd");
                            std::cout<<"This column_array: "+arr[i]+" not exists"<<std::endl;
                            return -1;
                        }
                        break;
                    }
                }
                rlp=rlp.substr(0,rlp.size()-3);
                if(rlp==""){
                    std::cout<<"This child are empty";
                    remove("/media/bound/db/tables/lock.bd");
                    return -1;
                }else{
                    if(rlp.substr(0,3)=="y_n"&&rlp.substr(6,7)[0]=='n'){
                        std::cout<<"Can't edit a child w/sub-child";
                        remove("/media/bound/db/tables/lock.bd");
                        return -1;
                    }else{
                        rlp=rlp.substr(rlp.find("pth")+6);
                        rlp=rlp.substr(0,rlp.size()-3);
                        std::string prty = openSafe("/media/bound/db/tables/"+tableName+"/yxy__xyx/"+rlp+".bd");
                        std::string opt=argv[5];
                        std::string _b=argv[6];
                        std::string vtx=prty;
                        if(opt=="specify"){
                            if(vtx.find("uuid_base:^:"+_b)==std::string::npos){
                                std::cout<<"this uuid_base does not exists";
                                remove("/media/bound/db/tables/lock.bd");
                                return -1;
                            }
                            vtx=vtx.substr((vtx.find("uuid_base:^:"+_b)-3),vtx.find("uuid_base:^:"+_b)).substr(0,3);
                            if(prty.find("uuid_base:^:"+_b)==std::string::npos){
                                std::cout<<"this uuid_base does not exists";
                                remove("/media/bound/db/tables/lock.bd");
                                return -1;
                            }
                            
                            if(vtx!="{>}"&&vtx!="^<>"){
                                std::cout<<"this uuid_base does not exists";
                                remove("/media/bound/db/tables/lock.bd");
                                return -1;
                            }
                            std::string bfrl=prty.substr(0,prty.find("uuid_base:^:"+_b));
                            vtx=prty.substr(prty.find("uuid_base:^:"+_b));
                            std::string afrl=vtx.substr(vtx.find("^<>"));                        
                            vtx=vtx.substr(0,vtx.find("^<>"));
                            vtx=vtx.substr(0,vtx.size());
                        }
                        std::cout<<vtx<<std::endl;
                        remove("/media/bound/db/tables/lock.bd");
                        return 1;
                    }
                }
            }
        }
    }else if(command=="erd_child"){
        if(argc!=8&&argc!=7&&argc!=9){
            std::cout << "Usage: /media/FileDash/bound erd_child <NameTable> <Identification> <name_column_array0,name_column_array1...> <uuid_base> <if edit: existent_parameter1:^:newdata1>|<new_parameter1:^:newdata2... if removeParam: existent_parameter1 if removeValue: -1... if add_param_array: nw_param:^:new_parameter> <{edit or removeParam or removeValue or add_param_array}>"<<std::endl;
            return -1;
        }
        std::string opt=argv[6];
        if(((opt=="add_param_array"||opt=="remove_param_array")&&argc!=8)||(argc!=9&&(opt=="edit_param_array"||opt=="delete_param_array"))||(argc!=7&&(opt=="edit"||opt=="removeParam"||opt=="removeValue"))){
            std::cout << "Usage: /media/FileDash/bound erd_child <NameTable> <Identification> <name_column_array0,name_column_array1...> <uuid_base> <if edit: existent_parameter1:^:newdata1>|<new_parameter1:^:newdata2... if removeParam: existent_parameter1 if removeValue: -1... if add_param_array: nw_param:^:new_parameter> <{edit or removeParam or removeValue or add_param_array}>"<<std::endl;
            return -1;
        }
        std::string tableName=argv[2];
        if(std::filesystem::exists("/media/bound/db/tables/"+tableName+"/erd_child.bd")){
            if(!std::filesystem::exists("/media/bound/db/tables/"+tableName)){
                std::string wis="/media/bound/db/tables/"+tableName+"/erd_child.bd";
                remove(wis.c_str());
                std::cout << "This table not exist" << std::endl;
                return -1;
            }
            lock();
            std::string prty = openSafe("/media/bound/db/tables/"+tableName+"/find.bd");
            std::stringstream ar;
            ar<<argv[3];
            ar<<"<!>";
            size_t nt=prty.find(ar.str());
            if(nt==std::string::npos){
                std::cout<<"This identification not exists"<<std::endl;
                std::string wis="/media/bound/db/tables/"+tableName+"/erd_child.bd";
                remove(wis.c_str());
                remove("/media/bound/db/tables/lock.bd");
                return 1;
            }else{
                std::string conttmp=prty;
                while(true){
                    size_t cn=conttmp.find(ar.str());
                    if(cn==std::string::npos){  
                        std::cout<<"This identification not exists"<<std::endl;
                        remove("/media/bound/db/tables/lock.bd");
                        std::string wis="/media/bound/db/tables/"+tableName+"/erd_child.bd";
                        remove(wis.c_str());
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
                    std::string invr = openSafe("/media/bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
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
                                std::string wis="/media/bound/db/tables/"+tableName+"/erd_child.bd";
                                remove(wis.c_str());
                                remove("/media/bound/db/tables/lock.bd");
                                std::cout<<"This column_array: "+arr[i]+" not exists"<<std::endl;
                                return -1;
                            }
                            break;
                        }
                    }
                    rlp=rlp.substr(0,rlp.size()-3);
                    if(rlp==""){
                        std::cout<<"This child are empty";
                        std::string wis="/media/bound/db/tables/"+tableName+"/erd_child.bd";
                        remove(wis.c_str());
                        remove("/media/bound/db/tables/lock.bd");
                        return -1;
                    }else{
                        if(rlp.substr(0,3)=="y_n"&&rlp.substr(6,7)[0]=='n'){
                            std::cout<<"Can't edit a child w/sub-child";
                            std::string wis="/media/bound/db/tables/"+tableName+"/erd_child.bd";
                            remove(wis.c_str());
                            remove("/media/bound/db/tables/lock.bd");
                            return -1;
                        }else{
                            rlp=rlp.substr(rlp.find("pth")+6);
                            rlp=rlp.substr(0,rlp.size()-3);
                            std::string prty = openSafe("/media/bound/db/tables/"+tableName+"/yxy__xyx/"+rlp+".bd");
                            std::string _b=argv[5];
                            if(prty.find("uuid_base:^:"+_b)==std::string::npos){
                                std::cout<<"this uuid_base does not exists";
                                std::string wis="/media/bound/db/tables/"+tableName+"/erd_child.bd";
                                remove(wis.c_str());
                                remove("/media/bound/db/tables/lock.bd");
                                return -1;
                            }
                                
                            std::string vtx=prty.substr((prty.find("uuid_base:^:"+_b)-3),prty.find("uuid_base:^:"+_b)).substr(0,3);
                            if(vtx!="{>}"&&vtx!="^<>"){
                                std::cout<<"this uuid_base does not exists";
                                std::string wis="/media/bound/db/tables/"+tableName+"/erd_child.bd";
                                remove(wis.c_str());
                                remove("/media/bound/db/tables/lock.bd");
                                return -1;
                            }
                            std::string bfrl=prty.substr(0,prty.find("uuid_base:^:"+_b));
                            vtx=prty.substr(prty.find("uuid_base:^:"+_b));
                            std::string afrl=vtx.substr(vtx.find("^<>"));                        
                            vtx=vtx.substr(0,vtx.find("^<>"));
                            vtx=vtx.substr(0,vtx.size());
                            std::string vp = openSafe("/media/bound/db/tables/"+tableName+"/erd_child.bd");
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
                                std::ofstream outputFile("/media/bound/db/tables/"+tableName+"/yxy__xyx/"+rlp+".bd");
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
                                            std::ofstream outputFile("/media/bound/db/tables/"+tableName+"/yxy__xyx/"+rlp+".bd");
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
                                std::ofstream outputFile("/media/bound/db/tables/"+tableName+"/yxy__xyx/"+rlp+".bd");
                                outputFile<<bfr<<b;
                                outputFile.close();
                            }
                            std::string wis="/media/bound/db/tables/"+tableName+"/erd_child.bd";
                            remove(wis.c_str());
                            remove("/media/bound/db/tables/lock.bd");
                            return 1;
                        }
                    }
                }
            }
        }
    }else if(command=="remove_param_colChild"){
        if(argc!=7){
            std::cout << "Usage: /media/FileDash/bound edit_param_child <NameTable> <Identification> <name_column_array0,name_column_array1...> <uuid_base> <existent_parameter1:^:newdata1>|<new_parameter1:^:newdata2...>"<<std::endl;
            return -1;
        }
        std::string tableName=argv[2];
        if(!std::filesystem::exists("/media/bound/db/tables/"+tableName)){
            std::cout << "This table not exist" << std::endl;
            return -1;
        }
        lock();
        std::string prty = openSafe("/media/bound/db/tables/"+tableName+"/find.bd");
        std::stringstream ar;
        ar<<argv[3];
        ar<<"<!>";
        size_t nt=prty.find(ar.str());
        if(nt==std::string::npos){
            std::cout<<"This identification not exists"<<std::endl;
            remove("/media/bound/db/tables/lock.bd");
            return 1;
        }else{
            std::string conttmp=prty;
            while(true){
                size_t cn=conttmp.find(ar.str());
                if(cn==std::string::npos){  
                    std::cout<<"This identification not exists"<<std::endl;
                    remove("/media/bound/db/tables/lock.bd");
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
                std::string invr = openSafe("/media/bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
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
                            remove("/media/bound/db/tables/lock.bd");
                            std::cout<<"This column_array: "+arr[i]+" not exists"<<std::endl;
                            return -1;
                        }
                        break;
                    }
                }
                rlp=rlp.substr(0,rlp.size()-3);
                if(rlp==""){
                    std::cout<<"This child are empty";
                    remove("/media/bound/db/tables/lock.bd");
                    return -1;
                }else{
                    if(rlp.substr(0,3)=="y_n"&&rlp.substr(6,7)[0]=='n'){
                        std::cout<<"Can't edit a child w/sub-child";
                        remove("/media/bound/db/tables/lock.bd");
                        return -1;
                    }else{
                        rlp=rlp.substr(rlp.find("pth")+6);
                        rlp=rlp.substr(0,rlp.size()-3);
                        std::string prty = openSafe("/media/bound/db/tables/"+tableName+"/yxy__xyx/"+rlp+".bd");
                        std::string _b=argv[5];
                        if(prty.find("uuid_base:^:"+_b)==std::string::npos){
                            std::cout<<"this uuid_base does not exists";
                            remove("/media/bound/db/tables/lock.bd");
                            return -1;
                        }
                        std::string vtx=prty.substr((prty.find("uuid_base:^:"+_b)-3),prty.find("uuid_base:^:"+_b)).substr(0,3);
                        if(vtx!="{<}"&&vtx!="^<>"){
                            std::cout<<"this uuid_base does not exists";
                            remove("/media/bound/db/tables/lock.bd");
                            return -1;
                        }
                        std::string bfrl=prty.substr(0,prty.find("uuid_base:^:"+_b));
                        vtx=prty.substr(prty.find("uuid_base:^:"+_b));
                        std::string afrl=vtx.substr(vtx.find("^<>"));                        
                        vtx=vtx.substr(0,vtx.find("^<>"));
                        vtx=vtx.substr(0,vtx.size());
                        std::cout<<vtx<<std::endl;
                        remove("/media/bound/db/tables/lock.bd");
                        return -1;
                    }
                }
            }
        }
    }else if(command=="find_param_array"){
        if(argc!=6){
            std::cout << "Usage: /media/FileDash/bound find_param_array <NameTable> <Identification> <name_column_array,sub_name_column_array,sub_sub_name_column_array...> <uuid_base>" << std::endl;
            return -1;
        }
        std::string tableName=argv[2];
        if(!std::filesystem::exists("/media/bound/db/tables/"+tableName)){
            std::cout << "This table not exist" << std::endl;
            return -1;
        }
        lock(); 
        std::string prty = openSafe("/media/bound/db/tables/"+tableName+"/find.bd");
        std::stringstream ar;
        ar<<argv[3];
        ar<<"<!>";
        size_t nt=prty.find(ar.str());
        if(nt==std::string::npos){
            std::cout<<"This identification not exists"<<std::endl;
            remove("/media/bound/db/tables/lock.bd");
            return 1;
        }else{
            std::string conttmp=prty;
            while(true){
                size_t cn=conttmp.find(ar.str());
                if(cn==std::string::npos){
                    std::cout<<"This identification not exists"<<std::endl;
                    remove("/media/bound/db/tables/lock.bd");
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
                std::string invr = openSafe("/media/bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
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
                    remove("/media/bound/db/tables/lock.bd");
                    return -1;
                }else{
                    std::string tmpinvr=invr;
                    while(true){
                        if(tmpinvr.find(cxc.str())==std::string::npos){
                            std::cout<<"This name_column_array not exists"<<std::endl;
                            remove("/media/bound/db/tables/lock.bd");
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
                                remove("/media/bound/db/tables/lock.bd");
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
                                        remove("/media/bound/db/tables/lock.bd");
                                        return -1;
                                    }
                                }
                                remove("/media/bound/db/tables/lock.bd");
                                return -1;
                            }
                        }
                    }
                }
            }
        }
    }else if(command=="edit_paramI_array"){
        if(argc!=6){
            std::cout << "Usage: /media/FileDash/bound edit_param_array <NameTable> <Identification> <name_column_array,sub_name_column_array,sub_sub_name_column_array...> <uuid_base>" << std::endl;
            return -1;
        }
        std::string tableName=argv[2];
        if(std::filesystem::exists("/media/bound/db/tables/"+tableName+"/edit_paramI_array.bd")){
            if(!std::filesystem::exists("/media/bound/db/tables/"+tableName)){
                std::cout << "This table not exist" << std::endl;
                std::string wis="/media/bound/db/tables/"+tableName+"/edit_paramI_array.bd";
                remove(wis.c_str());
                return -1;
            }
            lock();
            
            std::string prty = openSafe("/media/bound/db/tables/"+tableName+"/find.bd");
            std::stringstream ar;
            ar<<argv[3];
            ar<<"<!>";
            size_t nt=prty.find(ar.str());
            if(nt==std::string::npos){
                std::cout<<"This identification not exists"<<std::endl;
                remove("/media/bound/db/tables/lock.bd");
                std::string wis="/media/bound/db/tables/"+tableName+"/edit_paramI_array.bd";
                remove(wis.c_str());
                return 1;
            }else{
                std::string conttmp=prty;
                while(true){
                    size_t cn=conttmp.find(ar.str());
                    if(cn==std::string::npos){
                        std::cout<<"This identification not exists"<<std::endl;
                        remove("/media/bound/db/tables/lock.bd");
                        std::string wis="/media/bound/db/tables/"+tableName+"/edit_paramI_array.bd";
                        remove(wis.c_str());
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
                    std::string invr = openSafe("/media/bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
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
                    std::vector<std::string>arx=parse_dict(tl);
                    std::vector<bool>arxexs;
                    for(int i=0;i<arx.size();i++){
                        arxexs.push_back(false);
                    }
                    int arxpnt=0;
                    std::string xis="/media/bound/db/tables/"+tableName+"/edit_paramI_array.bd";
                    std::string invrbfr="";
                    std::string invrafr="";
                    std::vector<char>prm0={'!','@','!'};
                    int prm0pnt=0;
                    std::vector<char>prm1={'!','#','!'};
                    int prm1pnt=0;
                    std::vector<char>prmdc0={'>','|','<'};
                    int prmdc0pnt=0;
                    std::vector<char>prmdc1={'{','>','}'};
                    int prmdc1pnt=0;
                    std::vector<char>prmdc2={'{','<','}'};
                    int prmdc2pnt=0;
                    std::vector<char>prmdc3={'^','<','>'};
                    int prmdc3pnt=0;
                    int posPrm=0;
                    int cix=0;
                    int stpdc2=0;
                    int stparx=0;
                    int cix_=0;
                    int cixone=0;
                    bool skp=false;
                    /*
                        cix_ variable
                         current position from array right

                        cix variable
                         0 = nothing array using
                         -1 = using a not correspondent array / skip it
                         -2 = using a add_content_child / skip it
                         -3 = here we are inside correspondent array but it join in a new sub_array not correspondent with currently input then we'll inside in this to skip
                         1 = using a correspondent array
                         2 = using a correspondent array with sub_child but input 'uuid_base' from user is not equal with this content then need skip
                    */
                    int igf=0;
                    std::string pntdefine="";
                    int pos=-1;
                    std::string strr=argv[5];
                    std::vector<char>lco={':','^',':'};
                    int lcopnt=0;
                    int skc=0;
                    std::string skcstr="";
                    bool vtv=false;
                    for(int i=0;i<invr.size();i++){
                        if(skc==0){
                            if(prmdc0[prmdc0pnt]==invr[i]){
                                prmdc0pnt+=1;
                            }else{
                                prmdc0pnt=0;
                                if(prmdc0[prmdc0pnt]==invr[i]){
                                    prmdc0pnt+=1;
                                }
                            }
                            if(prmdc0pnt==3){
                                skc=1;
                                prmdc0pnt=0;
                            }
                        }else if(skc==1){
                            if(lco[lcopnt]==invr[i]){
                                lcopnt+=1;
                            }else{
                                lcopnt=0;
                                if(lco[lcopnt]==invr[i]){
                                    lcopnt+=1;
                                }
                            }
                            if(lcopnt==3){
                                skcstr=skcstr.substr(0,skcstr.size()-2);
                                if(skcstr=="uuid_base"){
                                    skc=2;
                                    skcstr="";
                                    lcopnt=0;
                                    continue;
                                }
                                skc=0;
                                skcstr="";
                                lcopnt=0;
                                continue;
                            }
                            skcstr+=invr[i];
                        }else{
                            if(prmdc0[prmdc0pnt]==invr[i]){
                                prmdc0pnt+=1;
                            }else{
                                prmdc0pnt=0;
                                if(prmdc0[prmdc0pnt]==invr[i]){
                                    prmdc0pnt+=1;
                                }
                            }
                            if(prmdc0pnt==3){
                                skcstr=skcstr.substr(0,skcstr.size()-2);
                                if(skcstr==strr){
                                    /*    
                                        std::vector<char>prm0={'!','@','!'};
                                        int prm0pnt=0;
                                        std::vector<char>prm1={'!','#','!'};
                                        int prm1pnt=0;
                                    */
                                    vtv=true;
                                    std::vector<char>prmdc3tmp={'>','<','^'};
                                    prm0pnt=0;
                                    prmdc3pnt=0;
                                    std::string brs="";
                                    int vj=0;
                                    prm1pnt=0;
                                    prm0pnt=0;
                                    for(int xy=i;xy<invr.size();xy--){
                                        if(prm1[prm1pnt]==invr[xy]){
                                            prm1pnt+=1;
                                        }else{
                                            prm1pnt=0;
                                            if(prm1[prm1pnt]==invr[xy]){
                                                prm1pnt+=1;
                                            }   
                                        }
                                        if(prm1pnt==3){
                                            prm0pnt=0;
                                            prm1pnt=0;
                                            prmdc3pnt=0;
                                        }
                                        if(prm0[prm0pnt]==invr[xy]){
                                            prm0pnt+=1;
                                        }else{
                                            prm0pnt=0;
                                            if(prm0[prm0pnt]==invr[xy]){
                                                prm0pnt+=1;
                                            }
                                        }
                                        if(prmdc3tmp[prmdc3pnt]==invr[xy]){
                                            prmdc3pnt+=1;
                                        }else{
                                            prmdc3pnt=0;
                                            if(prmdc3tmp[prmdc3pnt]==invr[xy]){
                                                prmdc3pnt+=1;
                                            }
                                        }
                                        brs+=invr[xy];
                                        if(prm0pnt==3||prmdc3pnt==3){
                                            prm1pnt=0;
                                            prm0pnt=0;
                                            vj=xy;
                                            vj+=3;
                                            break;
                                        }
                                    }
                                    prmdc3pnt=0;
                                    prm0pnt=0;
                                    i++;
                                    int paf=0;
                                    std::string btd="";
                                    int ard=0;
                                    int stn=0;
                                    prm1pnt=0;
                                    
                                    for(int x=i;x<invr.size();x++){
                                        btd+=invr[x];
                                        if(ard==0){
                                            if(prm0[prm0pnt]==invr[x]){
                                                prm0pnt+=1;
                                            }else{
                                                prm0pnt=0;
                                                if(prm0[prm0pnt]==invr[x]){
                                                    prm0pnt+=1;
                                                }   
                                            }
                                            if(prm0pnt==3){
                                                prm0pnt=0;
                                                prmdc3pnt=0;
                                                stn+=1;
                                                ard=1;
                                            }

                                            if(prmdc3[prmdc3pnt]==invr[x]){
                                                prmdc3pnt+=1;
                                            }else{
                                                prmdc3pnt=0;
                                                if(prmdc3[prmdc3pnt]==invr[x]){
                                                    prmdc3pnt+=1;
                                                }   
                                            }
                                            if(prmdc3pnt==3){
                                                prm0pnt=0;
                                                paf=x-2;
                                                break;
                                            }
                                        }else{
                                            if(prm0[prm0pnt]==invr[x]){
                                                prm0pnt+=1;
                                            }else{
                                                prm0pnt=0;
                                                if(prm0[prm0pnt]==invr[x]){
                                                    prm0pnt+=1;
                                                }   
                                            }
                                            if(prm0pnt==3){
                                                prm0pnt=0;
                                                prm1pnt=0;
                                                stn+=1;
                                            }
                                            if(prm1[prm1pnt]==invr[x]){
                                                prm1pnt+=1;
                                            }else{
                                                prm1pnt=0;
                                                if(prm1[prm1pnt]==invr[x]){
                                                    prm1pnt+=1;
                                                }   
                                            }
                                            if(prm1pnt==3){
                                                prm0pnt=0;
                                                prm1pnt=0;
                                                stn-=1;
                                            }
                                            if(stn==0){
                                                prm0pnt=0;
                                                prm1pnt=0;
                                                ard=0;
                                            }
                                        }
                                    }
                                    std::reverse(brs.begin(),brs.end());
                                    invrbfr=invr.substr(0,vj);
                                    invrafr=invr.substr(paf);
                                    invr=brs.substr(3,brs.size())+btd.substr(0,btd.size()-3);
                                }
                                skcstr="";
                                skc=0;
                                prmdc0pnt=0;
                                continue;
                            }
                            skcstr+=invr[i];
                        }
                        continue;
                    }
                    if(!vtv){
                        remove("/media/bound/db/tables/lock.bd");
                        std::string wis="/media/bound/db/tables/"+tableName+"/edit_paramI_array.bd";
                        remove(wis.c_str());
                        std::cout<<"You need use a valid uuid_base"<<std::endl;
                        return -1;
                    }
                    std::vector<std::string>inpsKy;
                    std::vector<int>posKy;
                    std::vector<std::string>inpsVl;
                    std::vector<int>posVl;
                    std::vector<std::string>posOutKy;
                    std::vector<int>posOutStr;
                    std::vector<int>posOutEnd;
                    skcstr="";
                    lcopnt=0;
                    prm0pnt=0;
                    prm1pnt=0;
                    prmdc0pnt=0;
                    stpdc2=0;
                    cix=0;
                    for(int i=0;i<invr.size();i++){
                        if(cix==0){
                            skcstr+=invr[i];
                            if(prmdc0[prmdc0pnt]==invr[i]){
                                prmdc0pnt+=1;
                            }else{
                                prmdc0pnt=0;
                                if(prmdc0[prmdc0pnt]==invr[i]){
                                    prmdc0pnt+=1;
                                }   
                            }
                            if(prmdc0pnt==3){
                                skcstr="";
                                prmdc0pnt=0;
                                prm0pnt=0;
                                lcopnt=0;
                            }
                            if(prm0[prm0pnt]==invr[i]){
                                prm0pnt+=1;
                            }else{
                                prm0pnt=0;
                                if(prm0[prm0pnt]==invr[i]){
                                    prm0pnt+=1;
                                }
                            }
                            if(prm0pnt==3){
                                skcstr="";
                                cix=2;
                                prm0pnt=0;
                                std::vector<char>prmdc0tmp={'<','|','>'};
                                for(int y=i;i<invr.size();y--){
                                    if(invr[y]==prmdc0tmp[prmdc0pnt]){
                                        prmdc0pnt+=1;
                                    }else{
                                        prmdc0pnt=0;
                                        if(invr[y]==prmdc0tmp[prmdc0pnt]){
                                            prmdc0pnt+=1;
                                        }
                                    }
                                    if(prmdc0pnt==3){
                                        lcopnt=0;
                                        prmdc0pnt=0;
                                        posOutStr.push_back(y);
                                        std::reverse(skcstr.begin(),skcstr.end());
                                        skcstr=skcstr.substr(0,skcstr.size()-3);
                                        skcstr=skcstr.substr(3,skcstr.size()-3);
                                        posOutKy.push_back(skcstr);
                                        skcstr="";
                                        break;
                                    }
                                    skcstr+=invr[y];
                                }
                                skcstr="";
                                stpdc2=1;
                            }
                            if(lco[lcopnt]==invr[i]){
                                lcopnt+=1;
                            }else{
                                lcopnt=0;
                                if(lco[lcopnt]==invr[i]){
                                    lcopnt+=1;
                                }   
                            }
                            if(lcopnt==3){
                                lcopnt=0;
                                prmdc0pnt=0;
                                skcstr=skcstr.substr(0,skcstr.size()-3);
                                inpsKy.push_back(skcstr);
                                posKy.push_back(i+1);
                                skcstr="";
                                cix=1;
                            }
                        }else if(cix==1){
                            if(prmdc0[prmdc0pnt]==invr[i]){
                                prmdc0pnt+=1;
                            }else{
                                prmdc0pnt=0;
                                if(prmdc0[prmdc0pnt]==invr[i]){
                                    prmdc0pnt+=1;
                                }
                            }
                            if(prmdc0pnt==3){
                                posVl.push_back(i-2);
                                inpsVl.push_back(skcstr.substr(0,skcstr.size()-2));
                                skcstr="";
                                cix=0;
                                continue;
                            }
                            skcstr+=invr[i];
                        }else if(cix==2){
                            if(prm0[prm0pnt]==invr[i]){
                                prm0pnt+=1;
                            }else{
                                prm0pnt=0;
                                if(prm0[prm0pnt]==invr[i]){
                                    prm0pnt+=1;
                                }
                            }
                            if(prm0pnt==3){
                                prm1pnt=0;
                                prm0pnt=0;
                                stpdc2+=1;
                            }
                            if(prm1[prm1pnt]==invr[i]){
                                prm1pnt+=1;
                            }else{
                                prm1pnt=0;
                                if(prm1[prm1pnt]==invr[i]){
                                    prm1pnt+=1;
                                }
                            }
                            if(prm1pnt==3){
                                prm1pnt=0;
                                prm0pnt=0;
                                stpdc2-=1;
                            }
                            if(stpdc2==0){
                                posOutEnd.push_back(i);
                                cix=0;
                                skcstr="";
                                continue;
                            }
                        }
                    }
                    if(cix==1){
                        posVl.push_back(invr.size());
                        inpsVl.push_back(skcstr);
                        cix=0;
                        skcstr="";
                    }
                    lcopnt=0;
                    prmdc0pnt=0;
                    std::string hm=openSafe(xis);
                    cix=0;
                    skcstr="";
                    std::vector<std::string>kysInp;
                    std::vector<std::string>vlsInp;
                    for(int i=0;i<hm.size();i++){
                        skcstr+=hm[i];
                        if(cix==0){
                            if(lco[lcopnt]==hm[i]){
                                lcopnt+=1;
                            }else{
                                lcopnt=0;
                                if(lco[lcopnt]==hm[i]){
                                    lcopnt+=1;
                                }   
                            }
                            if(lcopnt==3){
                                cix=1;
                                lcopnt=0;
                                skcstr=skcstr.substr(0,skcstr.size()-3);
                                kysInp.push_back(skcstr);
                                skcstr="";
                            }
                        }else{
                            if(prmdc0[prmdc0pnt]==hm[i]){
                                prmdc0pnt+=1;
                            }else{
                                prmdc0pnt=0;
                                if(prmdc0[prmdc0pnt]==hm[i]){
                                    prmdc0pnt+=1;
                                }
                            }
                            if(prmdc0pnt==3){
                                cix=0;
                                prmdc0pnt=0;
                                skcstr=skcstr.substr(0,skcstr.size()-3);
                                vlsInp.push_back(skcstr);
                                skcstr="";
                            }
                        }
                    }
                    vlsInp.push_back(skcstr);
                    for(int i=0;i<kysInp.size();i++){
                        if(kysInp[i]=="base"||kysInp[i]=="uuid_base"){
                            std::cout<<"You can't use base or uuid_base in input params"<<std::endl;
                            remove("/media/bound/db/tables/lock.bd");
                            std::string wis="/media/bound/db/tables/"+tableName+"/edit_paramI_array.bd";
                            remove(wis.c_str());
                            return -1;
                        }
                        for(int y=0;y<posOutKy.size();y++){
                            if(kysInp[i]==posOutKy[y]){
                                std::cout<<"This key: "<<kysInp[i]<<" is part from a sub_array in this content"<<std::endl;
                                remove("/media/bound/db/tables/lock.bd");
                                std::string wis="/media/bound/db/tables/"+tableName+"/edit_paramI_array.bd";
                                remove(wis.c_str());
                                return -1;
                            }
                        }
                        std::vector<int>posExs;
                        bool exs=false;
                        for(int z=0;z<inpsKy.size();z++){
                            if(kysInp[i]==inpsKy[z]){
                                exs=true;
                                posExs.push_back(posKy[z]);
                                posExs.push_back(posVl[z]);
                                bool pos=false;
                                for(int v=z;v<inpsKy.size();v++){
                                    if(!pos){
                                        if(inpsKy[z]==inpsKy[v]){
                                            pos=true;
                                        }
                                    }else{
                                        posKy[v]-=inpsVl[z].size();
                                        posKy[v]+=vlsInp[i].size();
                                        posVl[v]-=inpsVl[z].size();
                                        posVl[v]+=vlsInp[i].size();
                                    }
                                }
                                break;
                            }
                        }
                        if(exs){
                            std::string invrafr;
                            std::string invrbvf;
                            /*Quando uma posição é alterada básicamente temos um problema que a posição deve ser 
                            atualizada entretanto ela permanece a mesma de antes então os novos dados já vem corrompidos*/
                            for(int xa=0;xa<posExs[0];xa++){
                                invrbvf+=invr[xa];
                            }
                            for(int xa=posExs[1];xa<invr.size();xa++){
                                invrafr+=invr[xa];
                            }
                            
                            invr=invrbvf+vlsInp[i]+invrafr;
                        }else{
                            invr+=">|<"+kysInp[i]+":^:"+vlsInp[i];
                        }
                    }
                    std::ofstream outputFile("/media/bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                    outputFile<<bfr.str()<<invrbfr<<invr<<invrafr<<afr;
                    outputFile.close();
                    remove("/media/bound/db/tables/lock.bd");
                    std::string wis="/media/bound/db/tables/"+tableName+"/edit_paramI_array.bd";
                    remove(wis.c_str());
                    return 0;
                }
            }
        }
    }else if(command=="edit_param_array"){
        if(argc!=7){
            std::cout << "Usage: /media/FileDash/bound edit_param_array <NameTable> <Identification> <name_column_array> <base_identification> <user:^:DarkSouls_23>|<comment:^:Hello>" << std::endl;
            return -1;
        }
        std::string tableName=argv[2];
        if(!std::filesystem::exists("/media/bound/db/tables/"+tableName)){
            std::cout << "This table not exist" << std::endl;
            return -1;
        }
        lock();
        std::string prty = openSafe("/media/bound/db/tables/"+tableName+"/find.bd");
        std::stringstream ar;
        ar<<argv[3];
        ar<<"<!>";
        size_t nt=prty.find(ar.str());
        if(nt==std::string::npos){
            std::cout<<"This identification not exists"<<std::endl;
            remove("/media/bound/db/tables/lock.bd");
            return 1;
        }else{
            std::string conttmp=prty;
            while(true){
                size_t cn=conttmp.find(ar.str());
                if(cn==std::string::npos){
                    std::cout<<"This identification not exists"<<std::endl;
                    remove("/media/bound/db/tables/lock.bd");
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
                std::string invr = openSafe("/media/bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
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
                    remove("/media/bound/db/tables/lock.bd");
                    return -1;
                }else{
                    std::string tmpinvr=invr;
                    std::vector<std::string>glt={"","",""};
                    while(true){
                        if(tmpinvr.find(cxc.str())==std::string::npos){
                            std::cout<<"This name_column_array not exists"<<std::endl;
                            remove("/media/bound/db/tables/lock.bd");
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
                                remove("/media/bound/db/tables/lock.bd");
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
                        std::ofstream outputFile("/media/bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                        outputFile << bfr.str()<<glt[0]<<glt[1]<<glt[2]<<afr;
                        outputFile.close();
                        break;
                    }
                    remove("/media/bound/db/tables/lock.bd");
                    return -1;
                }
            }
        }
    }else if(command=="removeI_array"){
        if(argc!=6){
            std::cout << "Usage: /media/FileDash/bound edit_param_array <NameTable> <Identification> <name_column_array,sub_name_column_array,sub_sub_name_column_array...> <uuid_base>" << std::endl;
            return -1;
        }
        std::string tableName=argv[2];
        if(!std::filesystem::exists("/media/bound/db/tables/"+tableName)){
            std::cout << "This table not exist" << std::endl;
            return -1;
        }
        lock();
        std::string prty = openSafe("/media/bound/db/tables/"+tableName+"/find.bd");
        std::stringstream ar;
        ar<<argv[3];
        ar<<"<!>";
        size_t nt=prty.find(ar.str());
        if(nt==std::string::npos){
            std::cout<<"This identification not exists"<<std::endl;
            remove("/media/bound/db/tables/lock.bd");
            return 1;
        }else{
            std::string conttmp=prty;
            while(true){
                size_t cn=conttmp.find(ar.str());
                if(cn==std::string::npos){
                    std::cout<<"This identification not exists"<<std::endl;
                    remove("/media/bound/db/tables/lock.bd");
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
                std::string invr = openSafe("/media/bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
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
                std::vector<std::string>arx=parse_dict(tl);
                int arxpnt=0;
                std::string invrbfr="";
                std::string invrafr="";
                std::vector<char>prm0={'!','@','!'};
                int prm0pnt=0;
                std::vector<char>prm1={'!','#','!'};
                int prm1pnt=0;
                std::vector<char>prmdc0={'>','|','<'};
                int prmdc0pnt=0;
                std::vector<char>prmdc1={'{','>','}'};
                int prmdc1pnt=0;
                std::vector<char>prmdc2={'{','<','}'};
                int prmdc2pnt=0;
                std::vector<char>prmdc3={'^','<','>'};
                int prmdc3pnt=0;
                int posPrm=0;
                int cix=0;
                int stpdc2=0;
                int stparx=0;
                int cix_=0;
                int cixone=0;
                bool skp=false;
                /*
                    cix_ variable
                     current position from array right

                    cix variable
                     0 = nothing array using
                     -1 = using a not correspondent array / skip it
                     -2 = using a add_content_child / skip it
                     -3 = here we are inside correspondent array but it join in a new sub_array not correspondent with currently input then we'll inside in this to skip
                     1 = using a correspondent array
                     2 = using a correspondent array with sub_child but input 'uuid_base' from user is not equal with this content then need skip
                */
                int igf=0;
                std::string pntdefine="";
                int pos=-1;
                std::string strr=argv[5];
                std::vector<char>lco={':','^',':'};
                int lcopnt=0;
                int skc=0;
                std::string skcstr="";
                bool vtv=false;
                for(int i=0;i<invr.size();i++){
                    if(skc==0){
                        if(prmdc0[prmdc0pnt]==invr[i]){
                            prmdc0pnt+=1;
                        }else{
                            prmdc0pnt=0;
                            if(prmdc0[prmdc0pnt]==invr[i]){
                                prmdc0pnt+=1;
                            }
                        }
                        if(prmdc0pnt==3){
                            skc=1;
                            prmdc0pnt=0;
                        }
                    }else if(skc==1){
                        if(lco[lcopnt]==invr[i]){
                            lcopnt+=1;
                        }else{
                            lcopnt=0;
                            if(lco[lcopnt]==invr[i]){
                                lcopnt+=1;
                            }
                        }
                        if(lcopnt==3){
                            skcstr=skcstr.substr(0,skcstr.size()-2);
                            if(skcstr=="uuid_base"){
                                skc=2;
                                skcstr="";
                                lcopnt=0;
                                continue;
                            }
                            skc=0;
                            skcstr="";
                            lcopnt=0;
                            continue;
                        }
                        skcstr+=invr[i];
                    }else{
                        if(prmdc0[prmdc0pnt]==invr[i]){
                            prmdc0pnt+=1;
                        }else{
                            prmdc0pnt=0;
                            if(prmdc0[prmdc0pnt]==invr[i]){
                                prmdc0pnt+=1;
                            }
                        }
                        if(prmdc0pnt==3){
                            skcstr=skcstr.substr(0,skcstr.size()-2);
                            if(skcstr==strr){
                                /*    
                                    std::vector<char>prm0={'!','@','!'};
                                    int prm0pnt=0;
                                    std::vector<char>prm1={'!','#','!'};
                                    int prm1pnt=0;
                                */
                                vtv=true;
                                std::vector<char>prmdc3tmp={'>','<','^'};
                                prm0pnt=0;
                                prmdc3pnt=0;
                                std::string brs="";
                                int vj=0;
                                prm1pnt=0;
                                prm0pnt=0;
                                for(int xy=i;xy<invr.size();xy--){
                                    if(prm1[prm1pnt]==invr[xy]){
                                        prm1pnt+=1;
                                    }else{
                                        prm1pnt=0;
                                        if(prm1[prm1pnt]==invr[xy]){
                                            prm1pnt+=1;
                                        }   
                                    }
                                    if(prm1pnt==3){
                                        prm0pnt=0;
                                        prm1pnt=0;
                                        prmdc3pnt=0;
                                    }
                                    if(prm0[prm0pnt]==invr[xy]){
                                        prm0pnt+=1;
                                    }else{
                                        prm0pnt=0;
                                        if(prm0[prm0pnt]==invr[xy]){
                                            prm0pnt+=1;
                                        }
                                    }
                                    if(prmdc3tmp[prmdc3pnt]==invr[xy]){
                                        prmdc3pnt+=1;
                                    }else{
                                        prmdc3pnt=0;
                                        if(prmdc3tmp[prmdc3pnt]==invr[xy]){
                                            prmdc3pnt+=1;
                                        }
                                    }
                                    brs+=invr[xy];
                                    if(prm0pnt==3||prmdc3pnt==3){
                                        prm1pnt=0;
                                        prm0pnt=0;
                                        vj=xy;
                                        vj+=3;
                                        break;
                                    }
                                }
                                prmdc3pnt=0;
                                prm0pnt=0;
                                i++;
                                int paf=0;
                                std::string btd="";
                                int ard=0;
                                int stn=0;
                                prm1pnt=0;
                                    
                                for(int x=i;x<invr.size();x++){
                                    btd+=invr[x];
                                    if(ard==0){
                                        if(prm0[prm0pnt]==invr[x]){
                                            prm0pnt+=1;
                                        }else{
                                            prm0pnt=0;
                                            if(prm0[prm0pnt]==invr[x]){
                                                prm0pnt+=1;
                                            }   
                                        }
                                        if(prm0pnt==3){
                                            prm0pnt=0;
                                            prmdc3pnt=0;
                                            stn+=1;
                                            ard=1;
                                        }

                                        if(prmdc3[prmdc3pnt]==invr[x]){
                                            prmdc3pnt+=1;
                                        }else{
                                            prmdc3pnt=0;
                                            if(prmdc3[prmdc3pnt]==invr[x]){
                                                prmdc3pnt+=1;
                                            }   
                                        }
                                        if(prmdc3pnt==3){
                                            prm0pnt=0;
                                            paf=x+1;
                                            break;
                                        }
                                    }else{
                                        if(prm0[prm0pnt]==invr[x]){
                                            prm0pnt+=1;
                                        }else{
                                            prm0pnt=0;
                                            if(prm0[prm0pnt]==invr[x]){
                                                prm0pnt+=1;
                                            }   
                                        }
                                        if(prm0pnt==3){
                                            prm0pnt=0;
                                            prm1pnt=0;
                                            stn+=1;
                                        }
                                        if(prm1[prm1pnt]==invr[x]){
                                            prm1pnt+=1;
                                        }else{
                                            prm1pnt=0;
                                            if(prm1[prm1pnt]==invr[x]){
                                                prm1pnt+=1;
                                            }   
                                        }
                                        if(prm1pnt==3){
                                            prm0pnt=0;
                                            prm1pnt=0;
                                            stn-=1;
                                        }
                                        if(stn==0){
                                            prm0pnt=0;
                                            prm1pnt=0;
                                            ard=0;
                                        }
                                    }
                                }
                                std::reverse(brs.begin(),brs.end());
                                invrbfr=invr.substr(0,vj);
                                invrafr=invr.substr(paf);
                                invr=brs.substr(3,brs.size())+btd.substr(0,btd.size());
                            }
                            skcstr="";
                            skc=0;
                            prmdc0pnt=0;
                            continue;
                        }
                        skcstr+=invr[i];
                    }
                    continue;
                }
                if(!vtv){
                    remove("/media/bound/db/tables/lock.bd");
                    std::cout<<"You need use a valid uuid_base"<<std::endl;
                    return -1;
                }
                std::vector<std::string>inpsKy;
                std::vector<int>posKy;
                std::vector<std::string>inpsVl;
                std::vector<int>posVl;
                std::vector<std::string>posOutKy;
                std::vector<int>posOutStr;
                std::vector<int>posOutEnd;
                skcstr="";
                lcopnt=0;
                prm0pnt=0;
                prm1pnt=0;
                prmdc0pnt=0;
                stpdc2=0;
                cix=0;
                for(int i=0;i<invr.size();i++){
                    if(cix==0){
                        skcstr+=invr[i];
                        if(prmdc0[prmdc0pnt]==invr[i]){
                            prmdc0pnt+=1;
                        }else{
                            prmdc0pnt=0;
                            if(prmdc0[prmdc0pnt]==invr[i]){
                                prmdc0pnt+=1;
                            }   
                        }
                        if(prmdc0pnt==3){
                            skcstr="";
                            prmdc0pnt=0;
                            prm0pnt=0;
                            lcopnt=0;
                        }
                        if(prm0[prm0pnt]==invr[i]){
                            prm0pnt+=1;
                        }else{
                            prm0pnt=0;
                            if(prm0[prm0pnt]==invr[i]){
                                prm0pnt+=1;
                            }
                        }
                        if(prm0pnt==3){
                            skcstr="";
                            cix=2;
                            prm0pnt=0;
                            std::vector<char>prmdc0tmp={'<','|','>'};
                            for(int y=i;i<invr.size();y--){
                                if(invr[y]==prmdc0tmp[prmdc0pnt]){
                                    prmdc0pnt+=1;
                                }else{
                                    prmdc0pnt=0;
                                    if(invr[y]==prmdc0tmp[prmdc0pnt]){
                                        prmdc0pnt+=1;
                                    }
                                }
                                if(prmdc0pnt==3){
                                    lcopnt=0;
                                    prmdc0pnt=0;
                                    posOutStr.push_back(y);
                                    std::reverse(skcstr.begin(),skcstr.end());
                                    skcstr=skcstr.substr(0,skcstr.size()-3);
                                    skcstr=skcstr.substr(3,skcstr.size()-3);
                                    posOutKy.push_back(skcstr);
                                    skcstr="";
                                    break;
                                }
                                skcstr+=invr[y];
                            }
                            skcstr="";
                            stpdc2=1;
                        }
                        if(lco[lcopnt]==invr[i]){
                            lcopnt+=1;
                        }else{
                            lcopnt=0;
                            if(lco[lcopnt]==invr[i]){
                                lcopnt+=1;
                            }   
                        }
                        if(lcopnt==3){
                            lcopnt=0;
                            prmdc0pnt=0;
                            skcstr=skcstr.substr(0,skcstr.size()-3);
                            inpsKy.push_back(skcstr);
                            posKy.push_back(i+1);
                            skcstr="";
                            cix=1;
                        }
                    }else if(cix==1){
                        if(prmdc0[prmdc0pnt]==invr[i]){
                            prmdc0pnt+=1;
                        }else{
                            prmdc0pnt=0;
                            if(prmdc0[prmdc0pnt]==invr[i]){
                                prmdc0pnt+=1;
                            }
                        }
                        if(prmdc0pnt==3){
                            posVl.push_back(i-2);
                            inpsVl.push_back(skcstr.substr(0,skcstr.size()-2));
                            skcstr="";
                            cix=0;
                            continue;
                        }
                        skcstr+=invr[i];
                    }else if(cix==2){
                        if(prm0[prm0pnt]==invr[i]){
                            prm0pnt+=1;
                        }else{
                            prm0pnt=0;
                            if(prm0[prm0pnt]==invr[i]){
                                prm0pnt+=1;
                            }
                        }
                        if(prm0pnt==3){
                            prm1pnt=0;
                            prm0pnt=0;
                            stpdc2+=1;
                        }
                        if(prm1[prm1pnt]==invr[i]){
                            prm1pnt+=1;
                        }else{
                            prm1pnt=0;
                            if(prm1[prm1pnt]==invr[i]){
                                prm1pnt+=1;
                            }
                        }
                        if(prm1pnt==3){
                            prm1pnt=0;
                            prm0pnt=0;
                            stpdc2-=1;
                        }
                        if(stpdc2==0){
                            posOutEnd.push_back(i);
                            cix=0;
                            skcstr="";
                            continue;
                        }
                    }
                }
                if(cix==1){
                    posVl.push_back(invr.size());
                    inpsVl.push_back(skcstr);
                    cix=0;
                    skcstr="";
                }
                for(int i=0;i<tl.size();i++){
                    std::cout<<tl[i]<<std::endl;
                }
                prmdc3pnt=0;
                lcopnt=0;
                cix=0;
                prm0pnt=0;
                prm1pnt=0;
                stpdc2=0;
                std::string tmpbfr;
                std::string tmpafr;
                std::string curr;
                
                int posbfr;
                int posafr;
                bool frs=true;
                for(int i=0;i<invrafr.size();i++){
                    if(cix==3){
                        if(prm0[prm0pnt]==invrafr[i]){
                            prm0pnt+=1;
                        }else{
                            prm0pnt=0;
                            if(prm0[prm0pnt]==invrafr[i]){
                                prm0pnt+=1;
                            }
                        }
                        if(prm0pnt==3){
                            prm0pnt=0;
                            prm1pnt=0;
                            stpdc2+=1;
                        }

                        if(prm1[prm1pnt]==invrafr[i]){
                            prm1pnt+=1;
                        }else{
                            prm1pnt=0;
                            if(prm1[prm1pnt]==invrafr[i]){
                                prm1pnt+=1;
                            }
                        }
                        if(prm1pnt==3){
                            prm0pnt=0;
                            prm1pnt=0;
                            stpdc2-=1;
                        }
                        if(stpdc2==0){
                            cix=0;
                        }
                        continue;
                    }
                    if(prm0[prm0pnt]==invrafr[i]){
                        prm0pnt+=1;
                    }else{
                        prm0pnt=0;
                        if(prm0[prm0pnt]==invrafr[i]){
                            prm0pnt+=1;
                        }
                    }
                    if(prm0pnt==3){
                        cix=3;
                        prm0pnt=0;
                        prm1pnt=0;
                        stpdc2+=1;
                        continue;
                    }
                    if(frs){
                        if(lco[lcopnt]==invrafr[i]){
                            lcopnt+=1;
                        }else{
                            lcopnt=0;
                            if(lco[lcopnt]==invrafr[i]){
                                lcopnt+=1;
                            }
                        }
                        if(lcopnt==3){
                            cix=2;
                            posbfr=i+1;
                            prm0pnt=0;
                            prm1pnt=0;
                            lcopnt=0;
                            frs=false;
                        }
                    }else if(cix==0){
                        if(prmdc3[prmdc3pnt]==invrafr[i]){
                            prmdc3pnt+=1;
                        }else{
                            prmdc3pnt=0;
                            if(prmdc3[prmdc3pnt]==invrafr[i]){
                                prmdc3pnt+=1;
                            }
                        }
                        if(prmdc3pnt==3){
                            cix=1;
                            prm0pnt=0;
                            prm1pnt=0;
                            prmdc3pnt=0;
                        }
                    }else if(cix==1){
                        if(lco[lcopnt]==invrafr[i]){
                            lcopnt+=1;
                        }else{
                            lcopnt=0;
                            if(lco[lcopnt]==invrafr[i]){
                                lcopnt+=1;
                            }
                        }
                        if(lcopnt==3){
                            cix=2;
                            posbfr=i+1;
                            prm0pnt=0;
                            prm1pnt=0;
                            lcopnt=0;
                            frs=false;
                        }
                    }else{
                        if(invrafr[i]=='>'){
                            posafr=i;
                            std::string tmpinvrbfr;
                            std::string tmpinvrafr;
                            for(int y=0;y<posbfr;y++){
                                tmpinvrbfr+=invrafr[y];
                            }
                            for(int y=posafr;y<invrafr.size();y++){
                                tmpinvrafr+=invrafr[y];
                            }
                            int vxv=std::stoi(curr)-1;
                            invrafr=tmpinvrbfr+std::to_string(vxv)+tmpinvrafr;
                            curr="";
                            cix=0;
                            continue;
                        }
                        curr+=invrafr[i];
                    }
                }
                std::ofstream outputFile("/media/bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                outputFile<<bfr.str()<<invrbfr<<invrafr<<afr;
                outputFile.close();
                remove("/media/bound/db/tables/lock.bd");
                return 0;
            }
        }
    }else if(command=="remove_array"){
        if(argc!=6){
            std::cout << "Usage: /media/FileDash/bound edit_param_array <NameTable> <Identification> <name_column_array,sub_name_column_array,sub_sub_name_column_array...> <uuid_base>" << std::endl;
            return -1;
        }
        std::string tableName=argv[2];
        if(!std::filesystem::exists("/media/bound/db/tables/"+tableName)){
            std::cout << "This table not exist" << std::endl;
            return -1;
        }
        lock();
        std::string prty = openSafe("/media/bound/db/tables/"+tableName+"/find.bd");
        std::stringstream ar;
        ar<<argv[3];
        ar<<"<!>";
        size_t nt=prty.find(ar.str());
        if(nt==std::string::npos){
            std::cout<<"This identification not exists"<<std::endl;
            remove("/media/bound/db/tables/lock.bd");
            return 1;
        }else{
            std::string conttmp=prty;
            while(true){
                size_t cn=conttmp.find(ar.str());
                if(cn==std::string::npos){
                    std::cout<<"This identification not exists"<<std::endl;
                    remove("/media/bound/db/tables/lock.bd");
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
                std::string invr = openSafe("/media/bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
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
                std::vector<std::string>arx=parse_dict(tl);
                int arxpnt=0;
                std::string invrbfr="";
                std::string invrafr="";
                std::vector<char>prm0={'!','@','!'};
                int prm0pnt=0;
                std::vector<char>prm1={'!','#','!'};
                int prm1pnt=0;
                std::vector<char>prmdc0={'>','|','<'};
                int prmdc0pnt=0;
                std::vector<char>prmdc1={'{','>','}'};
                int prmdc1pnt=0;
                std::vector<char>prmdc2={'{','<','}'};
                int prmdc2pnt=0;
                std::vector<char>prmdc3={'^','<','>'};
                int prmdc3pnt=0;
                int posPrm=0;
                int cix=0;
                int stpdc2=0;
                int stparx=0;
                int cix_=0;
                int cixone=0;
                bool skp=false;
                /*
                    cix_ variable
                     current position from array right

                    cix variable
                     0 = nothing array using
                     -1 = using a not correspondent array / skip it
                     -2 = using a add_content_child / skip it
                     -3 = here we are inside correspondent array but it join in a new sub_array not correspondent with currently input then we'll inside in this to skip
                     1 = using a correspondent array
                     2 = using a correspondent array with sub_child but input 'uuid_base' from user is not equal with this content then need skip
                */
                int igf=0;
                std::string pntdefine="";
                int pos=-1;
                std::string strr=argv[5];
                std::vector<char>lco={':','^',':'};
                int lcopnt=0;
                int skc=0;
                std::string skcstr="";
                bool vtv=false;
                for(int i=0;i<invr.size();i++){
                    if(skc==0){
                        if(prm0[prm0pnt]==invr[i]){
                            prm0pnt+=1;
                        }else{
                            prm0pnt=0;
                            if(prm0[prm0pnt]==invr[i]){
                                prm0pnt+=1;
                            }
                        }
                        if(prm0pnt==3){
                            skc=1;
                            prmdc3pnt=0;
                            prm0pnt=0;
                            continue;
                        }
                        if(prmdc3[prmdc3pnt]==invr[i]){
                            prmdc3pnt+=1;
                        }else{
                            prmdc3pnt=0;
                            if(prmdc3[prmdc3pnt]==invr[i]){
                                prmdc3pnt+=1;
                            }
                        }
                        if(prmdc3pnt==3){
                            skc=1;
                            prmdc3pnt=0;
                            prm0pnt=0;
                        }
                    }else if(skc==1){
                        if(lco[lcopnt]==invr[i]){
                            lcopnt+=1;
                        }else{
                            lcopnt=0;
                            if(lco[lcopnt]==invr[i]){
                                lcopnt+=1;
                            }
                        }
                        if(lcopnt==3){
                            skcstr=skcstr.substr(0,skcstr.size()-2);
                            std::cout<<skcstr<<std::endl;
                            if(skcstr=="base"){
                                skc=2;
                                skcstr="";
                                lcopnt=0;
                                continue;
                            }
                            skc=0;
                            skcstr="";
                            lcopnt=0;
                            continue;
                        }
                        skcstr+=invr[i];
                    }else{
                        std::cout<<"opa"<<std::endl;
                        if(prmdc0[prmdc0pnt]==invr[i]){
                            prmdc0pnt+=1;
                        }else{
                            prmdc0pnt=0;
                            if(prmdc0[prmdc0pnt]==invr[i]){
                                prmdc0pnt+=1;
                            }
                        }
                        if(prmdc0pnt==3){
                            skcstr=skcstr.substr(0,skcstr.size()-2);
                            if(skcstr==strr){
                                /*    
                                    std::vector<char>prm0={'!','@','!'};
                                    int prm0pnt=0;
                                    std::vector<char>prm1={'!','#','!'};
                                    int prm1pnt=0;
                                */
                                vtv=true;
                                std::vector<char>prmdc3tmp={'>','<','^'};
                                prm0pnt=0;
                                prmdc3pnt=0;
                                std::string brs="";
                                int vj=0;
                                prm1pnt=0;
                                prm0pnt=0;
                                for(int xy=i;xy<invr.size();xy--){
                                    if(prm1[prm1pnt]==invr[xy]){
                                        prm1pnt+=1;
                                    }else{
                                        prm1pnt=0;
                                        if(prm1[prm1pnt]==invr[xy]){
                                            prm1pnt+=1;
                                        }   
                                    }
                                    if(prm1pnt==3){
                                        prm0pnt=0;
                                        prm1pnt=0;
                                        prmdc3pnt=0;
                                    }
                                    if(prm0[prm0pnt]==invr[xy]){
                                        prm0pnt+=1;
                                    }else{
                                        prm0pnt=0;
                                        if(prm0[prm0pnt]==invr[xy]){
                                            prm0pnt+=1;
                                        }
                                    }
                                    if(prmdc3tmp[prmdc3pnt]==invr[xy]){
                                        prmdc3pnt+=1;
                                    }else{
                                        prmdc3pnt=0;
                                        if(prmdc3tmp[prmdc3pnt]==invr[xy]){
                                            prmdc3pnt+=1;
                                        }
                                    }
                                    brs+=invr[xy];
                                    if(prm0pnt==3||prmdc3pnt==3){
                                        prm1pnt=0;
                                        prm0pnt=0;
                                        vj=xy;
                                        vj+=3;
                                        break;
                                    }
                                }
                                prmdc3pnt=0;
                                prm0pnt=0;
                                i++;
                                int paf=0;
                                std::string btd="";
                                int ard=0;
                                int stn=0;
                                prm1pnt=0;
                                    
                                for(int x=i;x<invr.size();x++){
                                    btd+=invr[x];
                                    if(ard==0){
                                        if(prm0[prm0pnt]==invr[x]){
                                            prm0pnt+=1;
                                        }else{
                                            prm0pnt=0;
                                            if(prm0[prm0pnt]==invr[x]){
                                                prm0pnt+=1;
                                            }   
                                        }
                                        if(prm0pnt==3){
                                            prm0pnt=0;
                                            prmdc3pnt=0;
                                            stn+=1;
                                            ard=1;
                                        }

                                        if(prmdc3[prmdc3pnt]==invr[x]){
                                            prmdc3pnt+=1;
                                        }else{
                                            prmdc3pnt=0;
                                            if(prmdc3[prmdc3pnt]==invr[x]){
                                                prmdc3pnt+=1;
                                            }   
                                        }
                                        if(prmdc3pnt==3){
                                            prm0pnt=0;
                                            paf=x+1;
                                            break;
                                        }
                                    }else{
                                        if(prm0[prm0pnt]==invr[x]){
                                            prm0pnt+=1;
                                        }else{
                                            prm0pnt=0;
                                            if(prm0[prm0pnt]==invr[x]){
                                                prm0pnt+=1;
                                            }   
                                        }
                                        if(prm0pnt==3){
                                            prm0pnt=0;
                                            prm1pnt=0;
                                            stn+=1;
                                        }
                                        if(prm1[prm1pnt]==invr[x]){
                                            prm1pnt+=1;
                                        }else{
                                            prm1pnt=0;
                                            if(prm1[prm1pnt]==invr[x]){
                                                prm1pnt+=1;
                                            }   
                                        }
                                        if(prm1pnt==3){
                                            prm0pnt=0;
                                            prm1pnt=0;
                                            stn-=1;
                                        }
                                        if(stn==0){
                                            prm0pnt=0;
                                            prm1pnt=0;
                                            ard=0;
                                        }
                                    }
                                }
                                std::reverse(brs.begin(),brs.end());
                                invrbfr=invr.substr(0,vj);
                                invrafr=invr.substr(paf);
                                invr=brs.substr(3,brs.size())+btd.substr(0,btd.size());
                            }
                            skcstr="";
                            skc=0;
                            prmdc0pnt=0;
                            continue;
                        }
                        skcstr+=invr[i];
                    }
                    continue;
                }
                if(!vtv){
                    remove("/media/bound/db/tables/lock.bd");
                    std::cout<<"You need use a valid uuid_base"<<std::endl;
                    return -1;
                }
                std::vector<std::string>inpsKy;
                std::vector<int>posKy;
                std::vector<std::string>inpsVl;
                std::vector<int>posVl;
                std::vector<std::string>posOutKy;
                std::vector<int>posOutStr;
                std::vector<int>posOutEnd;
                skcstr="";
                lcopnt=0;
                prm0pnt=0;
                prm1pnt=0;
                prmdc0pnt=0;
                stpdc2=0;
                cix=0;
                for(int i=0;i<invr.size();i++){
                    if(cix==0){
                        skcstr+=invr[i];
                        if(prmdc0[prmdc0pnt]==invr[i]){
                            prmdc0pnt+=1;
                        }else{
                            prmdc0pnt=0;
                            if(prmdc0[prmdc0pnt]==invr[i]){
                                prmdc0pnt+=1;
                            }   
                        }
                        if(prmdc0pnt==3){
                            skcstr="";
                            prmdc0pnt=0;
                            prm0pnt=0;
                            lcopnt=0;
                        }
                        if(prm0[prm0pnt]==invr[i]){
                            prm0pnt+=1;
                        }else{
                            prm0pnt=0;
                            if(prm0[prm0pnt]==invr[i]){
                                prm0pnt+=1;
                            }
                        }
                        if(prm0pnt==3){
                            skcstr="";
                            cix=2;
                            prm0pnt=0;
                            std::vector<char>prmdc0tmp={'<','|','>'};
                            for(int y=i;i<invr.size();y--){
                                if(invr[y]==prmdc0tmp[prmdc0pnt]){
                                    prmdc0pnt+=1;
                                }else{
                                    prmdc0pnt=0;
                                    if(invr[y]==prmdc0tmp[prmdc0pnt]){
                                        prmdc0pnt+=1;
                                    }
                                }
                                if(prmdc0pnt==3){
                                    lcopnt=0;
                                    prmdc0pnt=0;
                                    posOutStr.push_back(y);
                                    std::reverse(skcstr.begin(),skcstr.end());
                                    skcstr=skcstr.substr(0,skcstr.size()-3);
                                    skcstr=skcstr.substr(3,skcstr.size()-3);
                                    posOutKy.push_back(skcstr);
                                    skcstr="";
                                    break;
                                }
                                skcstr+=invr[y];
                            }
                            skcstr="";
                            stpdc2=1;
                        }
                        if(lco[lcopnt]==invr[i]){
                            lcopnt+=1;
                        }else{
                            lcopnt=0;
                            if(lco[lcopnt]==invr[i]){
                                lcopnt+=1;
                            }   
                        }
                        if(lcopnt==3){
                            lcopnt=0;
                            prmdc0pnt=0;
                            skcstr=skcstr.substr(0,skcstr.size()-3);
                            inpsKy.push_back(skcstr);
                            posKy.push_back(i+1);
                            skcstr="";
                            cix=1;
                        }
                    }else if(cix==1){
                        if(prmdc0[prmdc0pnt]==invr[i]){
                            prmdc0pnt+=1;
                        }else{
                            prmdc0pnt=0;
                            if(prmdc0[prmdc0pnt]==invr[i]){
                                prmdc0pnt+=1;
                            }
                        }
                        if(prmdc0pnt==3){
                            posVl.push_back(i-2);
                            inpsVl.push_back(skcstr.substr(0,skcstr.size()-2));
                            skcstr="";
                            cix=0;
                            continue;
                        }
                        skcstr+=invr[i];
                    }else if(cix==2){
                        if(prm0[prm0pnt]==invr[i]){
                            prm0pnt+=1;
                        }else{
                            prm0pnt=0;
                            if(prm0[prm0pnt]==invr[i]){
                                prm0pnt+=1;
                            }
                        }
                        if(prm0pnt==3){
                            prm1pnt=0;
                            prm0pnt=0;
                            stpdc2+=1;
                        }
                        if(prm1[prm1pnt]==invr[i]){
                            prm1pnt+=1;
                        }else{
                            prm1pnt=0;
                            if(prm1[prm1pnt]==invr[i]){
                                prm1pnt+=1;
                            }
                        }
                        if(prm1pnt==3){
                            prm1pnt=0;
                            prm0pnt=0;
                            stpdc2-=1;
                        }
                        if(stpdc2==0){
                            posOutEnd.push_back(i);
                            cix=0;
                            skcstr="";
                            continue;
                        }
                    }
                }
                if(cix==1){
                    posVl.push_back(invr.size());
                    inpsVl.push_back(skcstr);
                    cix=0;
                    skcstr="";
                }
                for(int i=0;i<tl.size();i++){
                    std::cout<<tl[i]<<std::endl;
                }
                prmdc3pnt=0;
                lcopnt=0;
                cix=0;
                prm0pnt=0;
                prm1pnt=0;
                stpdc2=0;
                std::string tmpbfr;
                std::string tmpafr;
                std::string curr;
                
                int posbfr;
                int posafr;
                bool frs=true;
                for(int i=0;i<invrafr.size();i++){
                    if(cix==3){
                        if(prm0[prm0pnt]==invrafr[i]){
                            prm0pnt+=1;
                        }else{
                            prm0pnt=0;
                            if(prm0[prm0pnt]==invrafr[i]){
                                prm0pnt+=1;
                            }
                        }
                        if(prm0pnt==3){
                            prm0pnt=0;
                            prm1pnt=0;
                            stpdc2+=1;
                        }

                        if(prm1[prm1pnt]==invrafr[i]){
                            prm1pnt+=1;
                        }else{
                            prm1pnt=0;
                            if(prm1[prm1pnt]==invrafr[i]){
                                prm1pnt+=1;
                            }
                        }
                        if(prm1pnt==3){
                            prm0pnt=0;
                            prm1pnt=0;
                            stpdc2-=1;
                        }
                        if(stpdc2==0){
                            cix=0;
                        }
                        continue;
                    }
                    if(prm0[prm0pnt]==invrafr[i]){
                        prm0pnt+=1;
                    }else{
                        prm0pnt=0;
                        if(prm0[prm0pnt]==invrafr[i]){
                            prm0pnt+=1;
                        }
                    }
                    if(prm0pnt==3){
                        cix=3;
                        prm0pnt=0;
                        prm1pnt=0;
                        stpdc2+=1;
                        continue;
                    }
                    if(frs){
                        if(lco[lcopnt]==invrafr[i]){
                            lcopnt+=1;
                        }else{
                            lcopnt=0;
                            if(lco[lcopnt]==invrafr[i]){
                                lcopnt+=1;
                            }
                        }
                        if(lcopnt==3){
                            cix=2;
                            posbfr=i+1;
                            prm0pnt=0;
                            prm1pnt=0;
                            lcopnt=0;
                            frs=false;
                        }
                    }else if(cix==0){
                        if(prmdc3[prmdc3pnt]==invrafr[i]){
                            prmdc3pnt+=1;
                        }else{
                            prmdc3pnt=0;
                            if(prmdc3[prmdc3pnt]==invrafr[i]){
                                prmdc3pnt+=1;
                            }
                        }
                        if(prmdc3pnt==3){
                            cix=1;
                            prm0pnt=0;
                            prm1pnt=0;
                            prmdc3pnt=0;
                        }
                    }else if(cix==1){
                        if(lco[lcopnt]==invrafr[i]){
                            lcopnt+=1;
                        }else{
                            lcopnt=0;
                            if(lco[lcopnt]==invrafr[i]){
                                lcopnt+=1;
                            }
                        }
                        if(lcopnt==3){
                            cix=2;
                            posbfr=i+1;
                            prm0pnt=0;
                            prm1pnt=0;
                            lcopnt=0;
                            frs=false;
                        }
                    }else{
                        if(invrafr[i]=='>'){
                            posafr=i;
                            std::string tmpinvrbfr;
                            std::string tmpinvrafr;
                            for(int y=0;y<posbfr;y++){
                                tmpinvrbfr+=invrafr[y];
                            }
                            for(int y=posafr;y<invrafr.size();y++){
                                tmpinvrafr+=invrafr[y];
                            }
                            int vxv=std::stoi(curr)-1;
                            invrafr=tmpinvrbfr+std::to_string(vxv)+tmpinvrafr;
                            curr="";
                            cix=0;
                            continue;
                        }
                        curr+=invrafr[i];
                    }
                }
                std::ofstream outputFile("/media/bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                outputFile<<bfr.str()<<invrbfr<<invrafr<<afr;
                outputFile.close();
                remove("/media/bound/db/tables/lock.bd");
                return 0;
            }
        }
    }else if(command=="append_array"){
        if(argc<5){
            std::cout << "Usage: /media/FileDash/bound append_array <NameTable> <Identification> <name_column_array>" << std::endl;
            return -1;
        }

        std::string tableName=argv[2];
        if(!std::filesystem::exists("/media/bound/db/tables/"+tableName)){
            std::cout << "This table not exist" << std::endl;
            std::string wis="/media/bound/db/tables/"+tableName+"/append.bd";
            remove(wis.c_str());
            return -1;
        }
        lock();
        std::string prty = openSafe("/media/bound/db/tables/"+tableName+"/find.bd");

        std::stringstream ar;
        ar<<argv[3];
        ar<<"<!>";
        size_t nt=prty.find(ar.str());
        if(nt==std::string::npos){
            std::cout<<"This identification not exists"<<std::endl;
            std::string wis="/media/bound/db/tables/"+tableName+"/append.bd";
            remove(wis.c_str());
            remove("/media/bound/db/tables/lock.bd");
            return -1;
        }else{
            std::string conttmp=prty;
            while(true){
                size_t cn=conttmp.find(ar.str());
                if(cn==std::string::npos){
                    std::cout<<"This identification not exists"<<std::endl;
                    std::string wis="/media/bound/db/tables/"+tableName+"/append.bd";
                    remove(wis.c_str());
                    remove("/media/bound/db/tables/lock.bd");
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
                std::string invr = openSafe("/media/bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
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
                    std::string wis="/media/bound/db/tables/"+tableName+"/append.bd";
                    remove(wis.c_str());
                    remove("/media/bound/db/tables/lock.bd");
                    return -1;
                }
                size_t arr=invr.find(argv[4]);
                std::vector<std::string>glt={"","",""};
                glt[0]=invr.substr(0,arr);
                std::string tmpchild=invr.substr(arr);
                if(tmpchild.find(":^:")==std::string::npos){
                    std::cout<<"This array not exists"<<std::endl;
                    std::string wis="/media/bound/db/tables/"+tableName+"/append.bd";
                    remove(wis.c_str());
                    remove("/media/bound/db/tables/lock.bd");
                    return -1;
                }
                size_t ar0=tmpchild.find(":^:");
                if(tmpchild.substr(ar0+3)[0]!='['){
                    std::cout<<"This array not exists"<<std::endl;
                    std::string wis="/media/bound/db/tables/"+tableName+"/append.bd";
                    remove(wis.c_str());
                    remove("/media/bound/db/tables/lock.bd");
                    return -1;
                } 
                if(tmpchild.substr(0,ar0)!=argv[4]){
                    std::cout<<"This array not exists"<<std::endl;
                    std::string wis="/media/bound/db/tables/"+tableName+"/append.bd";
                    remove(wis.c_str());
                    remove("/media/bound/db/tables/lock.bd");
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
                std::string append = openSafe("/media/bound/db/tables/"+tableName+"/append.bd");
                if(append.find("<|>")!=std::string::npos){
                    std::string wis="/media/bound/db/tables/"+tableName+"/append.bd";
                    remove(wis.c_str());
                    remove("/media/bound/db/tables/lock.bd");
                    return -1;
                }if(append.find(">|<")!=std::string::npos){
                    std::string wis="/media/bound/db/tables/"+tableName+"/append.bd";
                    remove(wis.c_str());
                    remove("/media/bound/db/tables/lock.bd");
                    return -1;
                }if(append.find("<!>")!=std::string::npos){
                    std::string wis="/media/bound/db/tables/"+tableName+"/append.bd";
                    remove(wis.c_str());
                    remove("/media/bound/db/tables/lock.bd");
                    return -1;
                }

                std::string gv=append;
                if(gv[0]!='['||gv[gv.size()-1]!=']'){
                    std::cout<<"new_data need be array"<<std::endl;
                    std::string wis="/media/bound/db/tables/"+tableName+"/append.bd";
                    remove(wis.c_str());
                    remove("/media/bound/db/tables/lock.bd");
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
                std::ofstream outputFile("/media/bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                outputFile << nwparam;
                outputFile.close();
                std::cout<<"Array updated"<<std::endl;
                std::string wis="/media/bound/db/tables/"+tableName+"/append.bd";
                remove(wis.c_str());
                remove("/media/bound/db/tables/lock.bd");
                return 0;                                           
            }
            std::cout<<"This identification not exists"<<std::endl;
            std::string wis="/media/bound/db/tables/"+tableName+"/append.bd";
            remove(wis.c_str());
            remove("/media/bound/db/tables/lock.bd");
            return -1;
        }
    }else if(command=="pop_arrayKV"){
        if(argc<6){
            std::cout << "Usage: /media/FileDash/bound pop_arrayKV <NameTable> <Identification> <column_array> <value_delete>" << std::endl;
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
        if(!std::filesystem::exists("/media/bound/db/tables/"+tableName)){
            std::cout << "This table not exist" << std::endl;
            return 1;
        }
        if(!std::filesystem::exists("/media/bound/db/tables/"+tableName+"/find.bd")){
            std::cout<<"This identification not exists"<<std::endl;
            return 1;
        }
        lock();
        std::string prty = openSafe("/media/bound/db/tables/"+tableName+"/find.bd");
        std::stringstream ar;
        ar<<argv[3];
        ar<<"<!>";
        size_t nt=prty.find(ar.str());
        if(nt==std::string::npos){
            std::cout<<"This identification not exists"<<std::endl;
            remove("/media/bound/db/tables/lock.bd");
            return -1;
        }else{
            std::string conttmp=prty;
            while(true){
                size_t cn=conttmp.find(ar.str());
                if(cn==std::string::npos){
                    std::cout<<"This identification not exists"<<std::endl;
                    remove("/media/bound/db/tables/lock.bd");
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
                std::string invr = openSafe("/media/bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
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
                    remove("/media/bound/db/tables/lock.bd");
                    return -1;
                }
                size_t arr=invr.find(argv[4]);
                std::vector<std::string>glt={"","",""};
                glt[0]=invr.substr(0,arr);
                std::string tmpchild=invr.substr(arr);
                if(tmpchild.find(":^:")==std::string::npos){
                    std::cout<<"This array not existxs"<<std::endl;
                    remove("/media/bound/db/tables/lock.bd");
                    return -1;
                }
                size_t ar0=tmpchild.find(":^:");
                if(tmpchild.substr(ar0+3)[0]!='['){
                    std::cout<<"This array not existws"<<std::endl;
                    remove("/media/bound/db/tables/lock.bd");
                    return -1;
                } 
                if(tmpchild.substr(0,ar0)!=argv[4]){
                    std::cout<<"This array not existfs"<<std::endl;
                    remove("/media/bound/db/tables/lock.bd");
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
                    remove("/media/bound/db/tables/lock.bd");
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
                std::ofstream outputFile("/media/bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                outputFile << bls.str();
                outputFile.close();
                remove("/media/bound/db/tables/lock.bd");
                return 0;
            }
        }
    }else if(command=="update"){
        if(argc<4){
            std::cout << "Usage: /media/FileDash/bound update <NameTable> <Identification> <existent_parameter1:^:newdata1>|<new_parameter1:^:newdata2...>" << std::endl;
            return -1;
        }

        std::string tableName=argv[2];
        if(std::filesystem::exists("/media/bound/db/tables/"+tableName+"/update.bd")){
            std::string tb=argv[3];        
            if(tableName.find("<|>")!=std::string::npos||tableName.find(">|<")!=std::string::npos||tableName.find("<!>")!=std::string::npos||
            tb.find("<|>")!=std::string::npos||tb.find(">|<")!=std::string::npos||tb.find("<!>")!=std::string::npos){
                std::string wis="/media/bound/db/tables/"+tableName+"/update.bd";
                remove(wis.c_str());
                return -1;
            }

            if(!std::filesystem::exists("/media/bound/db/tables/"+tableName)){
                std::cout << "This table not exist" << std::endl;
                std::string wis="/media/bound/db/tables/"+tableName+"/update.bd";
                remove(wis.c_str());
                return -1;
            }
            lock();
            std::string prty = openSafe("/media/bound/db/tables/"+tableName+"/find.bd");
            std::stringstream ar;
            ar<<argv[3];
            ar<<"<!>";
            size_t nt=prty.find(ar.str());
            if(nt==std::string::npos){
                std::cout<<"This identification not exists"<<std::endl;
                remove("/media/bound/db/tables/lock.bd");
                std::string wis="/media/bound/db/tables/"+tableName+"/update.bd";
                remove(wis.c_str());
                return -1;
            }else{
                std::string conttmp=prty;
                
                while(true){
                    size_t cn=conttmp.find(ar.str());
                    if(cn==std::string::npos){
                        std::cout<<"This identification not exists"<<std::endl;
                        remove("/media/bound/db/tables/lock.bd");
                        std::string wis="/media/bound/db/tables/"+tableName+"/update.bd";
                        remove(wis.c_str());
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
                    std::string invr = openSafe("/media/bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
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
                    std::string fileContent = openSafe("/media/bound/db/tables/"+tableName+"/config.bd");
                    size_t colsize=fileContent.find("identifier");
                    std::string col=fileContent.substr(colsize);
                    size_t colend=fileContent.find("}",colsize);
                    std::string arg = openSafe("/media/bound/db/tables/"+tableName+"/update.bd");
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
                                    remove("/media/bound/db/tables/lock.bd");
                                    std::string wis="/media/bound/db/tables/"+tableName+"/update.bd";
                                    remove(wis.c_str());
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
                            std::string tmpbfr=invr;
                            std::string b_c="";
                            while(true){
                                if(tmpbfr.find(i.substr(0,w))==std::string::npos){
                                    b_c+=tmpbfr;
                                    b_c+=">|<"+i;
                                    invr=b_c;
                                    std::cout<<"AAA"<<std::endl;
                                    std::cout<<invr<<std::endl;
                                    break;
                                }
                                std::string chkin=tmpbfr.substr(0,tmpbfr.find(i.substr(0,w)));
                                if(chkin.length()>=3){
                                    if(chkin.substr(chkin.length()-3)!=">|<"){
                                        b_c+=chkin;
                                        std::string af_p=tmpbfr.substr(tmpbfr.find(i.substr(0,w)));
                                        std::cout<<af_p<<std::endl;
                                        if(af_p.find(">|<")!=std::string::npos){
                                            b_c+=af_p.substr(0,af_p.find(">|<"));
                                            tmpbfr=af_p.substr(af_p.find(">|<"));
                                        }else{
                                            b_c+=af_p;
                                            tmpbfr="";
                                        }
                                    }else{
                                        size_t vle=tmpbfr.find(i.substr(0,w));
                                        std::string bfr=tmpbfr.substr(0,vle);
                                        std::string chk=tmpbfr.substr(vle);
                                        size_t wo=chk.find(":^:");
                                        size_t w=tmpbfr.find(chk.substr(0,wo));
                                        std::string aftr=tmpbfr.substr(w);
                                        size_t fts=aftr.find(":^:");
                                        aftr=aftr.substr(fts);
                                        if(aftr.find(">|<")!=std::string::npos){
                                            size_t ftr=aftr.find(">|<");
                                            aftr=aftr.substr(ftr);
                                        }else{
                                            aftr="";
                                        }
                                        invr=b_c+bfr+i+aftr;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    if(upfind){
                        std::ofstream outputFile("/media/bound/db/tables/"+tableName+"/find.bd");
                        outputFile << prty;
                        outputFile.close();
                    }
                    std::stringstream sn;
                    sn<<vec[0]<<invr<<vec[1];
                    std::ofstream outputFile("/media/bound/db/tables/"+tableName+"/"+currensm+"/"+path+".bd");
                    outputFile<<sn.str();
                    outputFile.close();
                    break;
                }
                remove("/media/bound/db/tables/lock.bd");
                std::string wis="/media/bound/db/tables/"+tableName+"/update.bd";
                remove(wis.c_str());
                return 0;
            }

        }
        return 0;
   }else if(command=="delete"){
        if(argc<4){
            std::cout << "Usage: /media/FileDash/bound delete <NameTable> <Identifier>" << std::endl;
            return -1;
        }
        std::string tableName=argv[2];
        std::string tb=argv[3];
        
        if(tableName.find("<|>")!=std::string::npos||tableName.find(">|<")!=std::string::npos||tableName.find("<!>")!=std::string::npos||
        tb.find("<|>")!=std::string::npos||tb.find(">|<")!=std::string::npos||tb.find("<!>")!=std::string::npos){
            return -1;
        }

        if(!std::filesystem::exists("/media/bound/db/tables/"+tableName)){
            std::cout << "This table not exist" << std::endl;
            return -1;
        }
        lock();
        std::string prty = openSafe("/media/bound/db/tables/"+tableName+"/find.bd");
        std::stringstream ar;
        ar<<argv[3];
        ar<<"<!>";
        size_t nt=prty.find(ar.str());
        if(nt==std::string::npos){
            std::cout<<"This identification not exists"<<std::endl;
            remove("/media/bound/db/tables/lock.bd");
            return -1;
        }else{
            std::string conttmp=prty;
            while(true){
                size_t cn=conttmp.find(ar.str());
                if(cn==std::string::npos){
                    std::cout<<"This identification not exists"<<std::endl;
                    remove("/media/bound/db/tables/lock.bd");
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
                kmp<<"/media/bound/db/tables/"<<tableName<<"/"<<currsm<<"/"<<ind<<".bd";
                std::string config=openSafe("/media/bound/db/tables/"+tableName+"/config.bd");

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
                    std::string contg =openSafe(kmp.str());
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
                std::ofstream outputFile("/media/bound/db/tables/"+tableName+"/find.bd");
                outputFile << findNw.str();
                outputFile.close();
                std::stringstream lst;
                lst<<"{\"currensm\":\""<<currensm<<"\"<|>\"qty\":\""<<qty<<"\"<|>\"indza\":\""<<indza<<"\"<|>\"sizeKb\":\""<<sizeKb<<"\"<|>\"identifier\":"<<ident;
                std::ofstream out0("/media/bound/db/tables/"+tableName+"/config.bd");
                out0 << lst.str();
                out0.close();
                remove("/media/bound/db/tables/lock.bd");
                return 0;
            }
        }
    }else {
        std::cout << "Unknown command: " << command << std::endl;
    }

    return 0;
}
