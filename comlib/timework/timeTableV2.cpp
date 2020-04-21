#include "timeTableV2.h"
#include "errHandle.h"
#include "tableWork.h"
#include <iostream>
#include <regex>

using namespace std;

static inline bool checkTime_type2(tm & reftm)
{
	return 1
		&& is_valid_daytim(reftm.tm_hour, reftm.tm_min, reftm.tm_sec)
		//&& is_valid_date(reftm.tm_year, reftm.tm_mon, reftm.tm_mday)
		;
}


ERR_STA load_table(char * filename, std::list<timTableSetV2> & outTable)
{
	ERR_STA err = err_UNKNOWN;
	cout << "load type 2" << endl;
	if (filename) {
		string loadbin;

		if ((err = loadFile(filename, loadbin)) == err_ok) {
			//匹配 15:14:13,-1.1,-2.2,3,4,5,6
			//匹配年月日，但不使用 也不对其正确性做判断
			regex reg("\\d{1,2}:\\d{1,2}:\\d{1,2}(?:,-?\\d*(?:\\.\\d+)?){6}");
			smatch match;


			string::const_iterator star = loadbin.begin();
			string::const_iterator end = loadbin.end();
			while (regex_search(star, end, match, reg)) {
#if 0
				int num = match.size();
				cout << "size = " << num << endl;
				for (int i = 0; i < num; i++) {
					cout << match[i] << endl;
				}
				cout << "----------------------------" << endl;
				/* 典型值
				size = 5
				2018 - 05 - 14 05:14 : 50, 106.213, 80.263, -0.275194, 0.946397, 0.169119
				05
				14
				50
				, 106.213, 80.263, -0.275194, 0.946397, 0.169119
				*/
#endif
				timTableSetV2 rtm;

				tm  thisTm;
				GetTim(thisTm);

				char *pos = (char *)&(*match[0].first);
				int cnt;
				if ((cnt = sscanf(pos, "%d:%d:%d,%f,%f,%d,%d,%d,%d"
					, &thisTm.tm_hour, &thisTm.tm_min, &thisTm.tm_sec
					, &rtm.ZxAng, &rtm.YxAng
					, &rtm.mdc_work_length, &rtm.mdc_mod, &rtm.weigth, &rtm.cap_reserve
				)) == 9) {
					if (checkTime_type2(thisTm)) {
						rtm.tt = mktime(&thisTm);
						outTable.emplace_back(rtm);
					} else {
						SN1V2_ERR_LOG("time check error,%d:%d:%d,%f,%f,%d,%d,%d,%d"
							, thisTm.tm_hour, thisTm.tm_min, thisTm.tm_sec
							, rtm.ZxAng, rtm.YxAng
							, rtm.mdc_work_length, rtm.mdc_mod, rtm.weigth, rtm.cap_reserve
						);
					}
				} else {
					SN1V2_ERR_LOG("scanf cnt = %d", cnt);
				}
				star = match[0].second;

			}
		} else {
			SN1V2_ERROR_MWSSAGE_WITH("load err", err);
			return err;
		}
	} else {
		SN1V2_ERROR_CODE_RET(err_inval_path);
	}
	return err;
}

static void add2list(list<timTableSetV2> & inputlist, timTableSetV2 & NewNode);
//itr 为 list 中下一个比较的节点
//testNode为添加的节点
//向后检测节点冲突
static void listCheck(list<timTableSetV2> & inputlist, list<timTableSetV2>::iterator & itr, list<timTableSetV2>::iterator & testNode)
{
	if (itr == inputlist.end()) {
		return;
	}

	int itrTim[2] = { itr->tt,itr->tt + itr->mdc_work_length };
	int tesTim[2] = { testNode->tt,testNode->tt + testNode->mdc_work_length };


	if (testNode->weigth > itr->weigth)
	{
		//添加的test 权重高
		if (itrTim[1] > tesTim[1]) {		
			//itr 时间长  仅需修改itr时间
			itr->mdc_work_length = itrTim[1] - tesTim[1];
			itr->tt = tesTim[1];
			return;
		} else {
			//test 时间长 
			//删除itr节点并迭代后续节点
			auto next = itr;
			next++;
			inputlist.erase(itr);
			listCheck(inputlist, next, testNode);
			return;
		}
	}
	else {
		//添加的test 权重低
		if (itrTim[1] > tesTim[1]) {
			//itr 时间长  仅需修改test时间
			testNode->mdc_work_length -= tesTim[1] - itrTim[0];
			return;
		} else {
			//test 时间长 
			//断裂 test节点 塞入itr并迭代
			testNode->mdc_work_length -= tesTim[1] - itrTim[0];
			
			timTableSetV2 testpart2 = *testNode;
			testpart2.tt = itrTim[1];
			testpart2.mdc_work_length = tesTim[1] - itrTim[1];
			add2list(inputlist, testpart2);
			return;
		}
	}
}


static void add2list(list<timTableSetV2> & inputlist, timTableSetV2 & NewNode)
{
	if (inputlist.empty()) {
		inputlist.emplace_back(NewNode);
	}

	list<timTableSetV2>::iterator first = inputlist.end();

	while (--first != inputlist.end()) {
		if (first->tt > NewNode.tt) {
			continue;
		}

		int oldtt[2] = { first->tt,first->tt + first->mdc_work_length };
		int newtt[2] = { NewNode.tt,NewNode.tt + NewNode.mdc_work_length };

		//检测覆盖
		if (oldtt[1] > newtt[0]) {
			//一共12种情况
			//1-6		旧权重高于新权重
			//6-12		新权重高于旧权重

			//1、2、3		7、8、9 不同一个起点
			//4、5、6		10、11、12 新旧同一个起点

			//1、4、7、10旧终点大于新终点
			//2、5、8、11旧终点等于新终点
			//3、6、9、12旧终点小于新终点


			//有覆盖情况
			//检测权重
			if (first->weigth > NewNode.weigth) {
				//情况1、2、4、5
				//完全覆盖 不用添加节点
				if (oldtt[1] >= newtt[1]) {
					return;
				}
				//情况3、6
				auto next = first;
				next++;
				NewNode.tt = oldtt[1];
				NewNode.mdc_work_length = newtt[1] - oldtt[1];
				auto chk = inputlist.insert(next, NewNode);
				listCheck(inputlist, next, chk);
				return;
			} else {
				if (oldtt[0] != newtt[0]) {
					if (oldtt[1] > newtt[1]) {
						//情况 7
						//old 断开 插入new
						//复制旧节点
						timTableSetV2 oldpart2 = *first;
						//调整旧节点
						first->mdc_work_length -= (oldtt[1] - newtt[0]);
						//插入新节点
						auto next = first;
						next++;
						inputlist.insert(next, NewNode);
						//调整旧节点part2
						oldpart2.tt = newtt[1];
						oldpart2.mdc_work_length = oldtt[1] - newtt[1];
						inputlist.insert(next, oldpart2);
						return;
					} else {
						//情况 8、9
						//调整旧节点 
						first->mdc_work_length -= (oldtt[1] - newtt[0]);
						//插入新节点
						auto next = first;
						next++;
						auto chk = inputlist.insert(next, NewNode);
						if (newtt[1] != oldtt[1]) {
							//情况9 需要向后审查
							listCheck(inputlist, next, chk);
						}
						return;
					}

				} else {
					if (newtt[1] < oldtt[1]) {
						//情况10
						//向前插入新节点
						inputlist.insert(first, NewNode);
						//修改旧节点
						first->tt = newtt[1];
						first->mdc_work_length = oldtt[1] - newtt[1];
						return;
					} else {
						//情况11、12 直接覆盖
						*first = NewNode;
						if (newtt[1] != oldtt[1]) {
							//情况12
							auto next = first;
							next++;
							listCheck(inputlist, next, first);
						}
						return;
					}
				}
			}
		} else {
			//没有覆盖
			inputlist.emplace_back(NewNode);
			return;
		}
	}
}


void FixTimeTableV2(list<timTableSetV2> & inputList)
{
	inputList.sort([](timTableSetV2 & a, timTableSetV2 & b) {return a.tt < b.tt; });

	list<timTableSetV2> tmplist;
	for (auto & p : inputList) {
		add2list(tmplist, p);
	}
	swap(tmplist, inputList);
}


void printTable(list<timTableSetV2> & reflist)
{
	for (auto &p : reflist) {
		time_t tt = p.tt;
		tm ref;
		localtime_r(&tt, &ref);

		printf("start tim %d:%d:%d  deg=%f,%f  worklen %d workmod %d weight %d cap %d\n ",
			ref.tm_hour, ref.tm_min, ref.tm_sec,
			p.ZxAng, p.YxAng,
			p.mdc_work_length, p.mdc_mod, p.weigth, p.cap_reserve
		);
	}
}
#if 1
void testList()
{
	list<int> a;
	a.push_back(1);
	a.push_back(2);
	a.push_back(3);

	auto itr = a.end();

	while (--itr != a.end()) {
		printf("pp = %d\n", *itr);
	}
}



#endif

int testTimeTableV2(int argc, char * argv[])
{
	list<timTableSetV2>  b1;
	load_table("test/b1.txt", b1);
	printf("scanf p1\n");
	printTable(b1);

	list<timTableSetV2>  b2;
	load_table("test/b2.txt", b2);
	printf("scanf p2\n");
	printTable(b2);

	list<timTableSetV2>  b3;
	load_table("test/b3.txt", b3);
	printf("scanf p3\n");
	printTable(b3);

	list<timTableSetV2>  all;
	auto _merg = [](timTableSetV2 a, timTableSetV2 b) {return true; };
	all.merge(b1, _merg);
	all.merge(b2, _merg);
	all.merge(b3, _merg);
	printf("all\n");
	printTable(all);



	FixTimeTableV2(all);
	printf("all fix 1\n");
	printTable(all);

	//testList();

	return 0;
}
