#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<conio.h>
#include"dllist.h"
#include"jrb.h"
#include"jval.h"

typedef struct{
	JRB edges;
	JRB vertices;
}Graph;

int countVertices = 0;

Graph createGraph();
void addVertex(Graph g, char *v);
void addEdge(Graph g, char *v1, char *v2, char *route);
int hasEdge(Graph g, char *v1, char *v2);
int indegree(Graph g, char *v, char output[][100]);
int outdegree(Graph g, char *v, char output[][100]);
void dropGraph(Graph g);

//RETURN A LIST OF POSSIBLE ROUTES FROM v1 TO v2
Dllist getRoute(Graph g, char *v1, char *v2);
//RETURN A DUPLICATE LIST FROM A GIVEN LIST d
Dllist copyList(Dllist d);
int existPath(Graph g, char *start, char *stop);
void findPath(Graph g, char *start, char *stop, int numberofpaths);

void readFile(Graph g){
	char *s = (char*) malloc (1000 * sizeof(char));
	char *buffer = (char*) malloc (1000 * sizeof(char));
	char *curRoute, *prevNode, *curNode;
	FILE *filein = fopen("dataset_refined.txt", "r");
	
	while(fgets(s, 1000, filein) != NULL){
		buffer = strtok(s, "-:[]\n");
		if (strcmp(buffer, "TUYEN ") == 0){
			buffer = strtok(NULL, "-:[]\n");
			curRoute = strdup(buffer);
//			printf("\n%s : \n", curRoute);
			continue;
		}
		else if (strcmp(buffer, "FORWARD") == 0 || strcmp(buffer, "BACKWARD") == 0){
//			printf("%s\n", buffer);
			prevNode = strtok(NULL, "-:[]\n");
//    		printf("  %s\n", prevNode);
			addVertex(g, prevNode);
			while((curNode = strtok(NULL, "-:[]\n")) != NULL){
//				printf("  %s\n", curNode);
				addVertex(g, curNode);
				addEdge(g, prevNode, curNode, curRoute);
				prevNode = strdup(curNode);
			}
		}
	}
}

void MENU(){
	printf("\n\tHA NOI BUSGRAPH\n");
	printf("---------------------------------\n\n");
	printf("1. FIND PATHs WITH SHORTEST ROUTE\n\n");
	printf("2. EXIT\n\n");
	printf("---------------------------------\n\n");
}

void main(){
	int n, check = 1;
	char output[100][100];
	JRB node, node1, node2, tree;
	int choice;
	Graph g = createGraph();
	readFile(g);
	char start[50], stop[50];
	
	while(check != 0){
		system("cls");
		MENU();
		printf("Enter choice : ");
		scanf("%d", &choice);
		if (choice == 1){
			printf("Enter start station: ");
			scanf(" %[^\n]", start);
			printf("Enter stop station: ");
			scanf(" %[^\n]", stop);
			if (!existPath(g, strupr(start), strupr(stop))) printf("No path between %s and %s!!!\n", strupr(start), strupr(stop));
			else{
				//MAXIMUM PATH NUMBER SHOULD BE AS SMALL AS POSSIBLE, n < 50 IS RECOMMENDED:<
				//OR findPath WILL TRAVERSE ALL POSSIBLE PATHS UNTIL IT GETS ENOUGH n
				//THE HIGHER n IS, THE MORE PRECISELY WE COULD GET THE PATH THAT SHOWS THE MINIMUM SWITCH TIMES
				printf("Enter the maximum number of paths : ");
				scanf("%d", &n);
				findPath(g, strupr(start), strupr(stop), n);
			}
			getch();
		}
		else if (choice ==2){
			printf("PROGRAM EXITTING!!!\n");
			dropGraph(g);
			break;
		}
		else{
			printf("???\n");
			getch();
		} 
	}
}

Graph createGraph(){
	Graph g;
	g.edges = make_jrb();
	g.vertices = make_jrb();
	return g;
}

void addVertex(Graph g, char *v){
	JRB node = jrb_find_str(g.vertices, v);
	if (node == NULL){
		jrb_insert_str(g.vertices, strdup(v), new_jval_i(1));
		countVertices += 1;
	}
}

//g :         key : v1,    val : adjList,
//adjList :   key : v2,    val : routeList,
//routeList : key : route, val : 1
void addEdge(Graph g, char *v1, char *v2, char *route){
	int routeExist = 0;
	JRB node1, node2, node3, adjList, routeList;
	//FIND v1
	node1 = jrb_find_str(g.edges, v1);
	//IF v1 NOT FOUND
	if (node1 == NULL){
		adjList = make_jrb();
		jrb_insert_str(g.edges, strdup(v1), new_jval_v(adjList));
		routeList = make_jrb();
		jrb_insert_str(adjList, strdup(v2), new_jval_v(routeList));
		jrb_insert_str(routeList, strdup(route), new_jval_i(1));
	}
	else{
		adjList = (JRB) jval_v(node1->val);
		//FIND v2
		node2 = jrb_find_str(adjList, v2);
		//IF v2 NOT FOUND
		if (node2 == NULL){
			routeList = make_jrb();
			jrb_insert_str(adjList, strdup(v2), new_jval_v(routeList));
			jrb_insert_str(routeList, strdup(route), new_jval_i(1));
		}
		else{
			routeList = (JRB) jval_v(node2->val);
			//FIND route
			node3 = jrb_find_str(routeList, route);
			//IF route NOT FOUND
			if (node3 == NULL){
				jrb_insert_str(routeList, strdup(route), new_jval_i(1));
			}
		}
	}
}

int hasEdge(Graph g, char *v1, char *v2){
	JRB node, adjList;
	node = jrb_find_str(g.edges, v1);
	if (node == NULL) return 0;
	else{
		adjList = (JRB) jval_v(node->val);
		if (jrb_find_str(adjList, v2) == NULL) return 0;
		else return 1;
	}
}

Dllist getRoute(Graph g, char *v1, char *v2){
	JRB node1, node2, node3, adjList, routeList;
	Dllist listRoute = new_dllist();
	if (hasEdge(g, v1, v2)){
		node1 = jrb_find_str(g.edges, v1);
		adjList = (JRB) jval_v(node1->val);
		node2 = jrb_find_str(adjList, v2);
		routeList = (JRB) jval_v(node2->val);
		jrb_traverse(node3, routeList){
			dll_append(listRoute, new_jval_s(strdup(node3->key.s)));
		}
	}
	return listRoute;
}

int indegree(Graph g, char *v, char output[][100]){
	JRB tree, node;
	int total = 0;
	jrb_traverse(node, g.edges){
		tree = (JRB) jval_v(node->val);
		if (jrb_find_str(tree, strdup(v))){
			strcpy(output[total++], jval_s(node->key));
		}
	}
	return total;
}

int outdegree(Graph g, char *v, char output[][100]){
	JRB node, node2, tree;
    JRB visited = make_jrb();
	int total = 0;
	node = jrb_find_str(g.edges, strdup(v));
	if (node == NULL) return 0;
	else{
		tree = (JRB) jval_v(node->val);
		jrb_traverse(node2, tree){
			if (!jrb_find_str(visited, jval_s(node2->key))){
				strcpy(output[total++], jval_s(node2->key));
				jrb_insert_str(visited, strdup(jval_s(node2->key)), new_jval_i(1));
			}
		}
		return total;
	}
}

//RETURN 1 IF v IS NOT IN PATH
int isNotVisited(Dllist path, char *v){
	Dllist node;
	dll_traverse(node, path){
		if (strcmp(jval_s(node->val), v) == 0){
			return 0;
		}
	}
	return 1;
}

void printPath(Dllist path){
	Dllist node;
	dll_traverse(node, path){
		printf("->%s ", jval_s(node->val));
	}
	printf("\n");
}

Dllist copyList(Dllist d){
	Dllist node, newpath;
	char *cur;
	newpath = new_dllist();
	dll_traverse(node, d){
		cur = strdup(jval_s(node->val));
		dll_append(newpath, new_jval_s(cur));
	}
	return newpath;
}

int countList(Dllist d1){
	int count = 0;
	Dllist node;
	dll_traverse(node, d1){
		count++;
	}
	return count;
}

//COMPARE 2 Dllists AND RETURN two Dllists, sameNode AND diffNode
//sameNode : the list of all same elements in both input dllists
//diffNode : the list of all different elements in both input dllists
void compare(Dllist d1, Dllist d2, Dllist sameNode, Dllist diffNode){
	int Check1;
	Dllist nodex, nodey;
	dll_traverse(nodex, d1){
		Check1 = 0;
		dll_traverse(nodey, d2){
			if(strcmp(jval_s(nodex->val), jval_s(nodey->val)) == 0){
				dll_append(sameNode, new_jval_s(strdup(jval_s(nodex->val))));
				Check1 = 1;
				break;
			}
		}
		if(!Check1){
			dll_append(diffNode, new_jval_s(strdup(jval_s(nodex->val))));
		}
	}
	Dllist nodez, nodeq;
	int Check2;
	dll_traverse(nodez, d2){
		Check2 = 0;
		dll_traverse(nodeq, sameNode){
			if(strcmp(jval_s(nodez->val), jval_s(nodeq->val)) == 0){
				Check2 = 1;
				break;
			}
		}
		if(!Check2){
			dll_append(diffNode, new_jval_s(strdup(jval_s(nodez->val))));
		}
	}
}

//RETURN 1 IF 2 LISTS ARE IDENTICAL (IN ORDER)
int identicalList(Dllist d1, Dllist d2){
	Dllist ptr1 = dll_first(d1);
	Dllist ptr2 = dll_first(d2);
	while(ptr1 != d1 && ptr2 != d2){
		if (strcmp(ptr1->val.s, ptr2->val.s) != 0) return 0;
		ptr1 = ptr1->flink;
		ptr2 = ptr2->flink;
	}
	return 1;
}

//RETURN listRoutes as A LIST OF ROUTES THAT SHOULD BE TAKEN (MIN SWITCH TIMES), NOT OPTIMAL YET
Dllist findMinRoutes(Graph g, Dllist path){
	char *start = strdup(dll_first(path)->val.s);
	char *stop = strdup(dll_last(path)->val.s);
	
	Dllist curNode, nextNode;
	Dllist listRoutes = new_dllist();
	Dllist curRouteList, prevRouteList;
	Dllist ptr1, ptr2, ptr3, ptr4;
	
	curNode = dll_first(path);
	while(curNode->flink != path){
		nextNode = curNode->flink;
		//IF curNode IS THE FIRST NODE IN PATH, JUST APPEND THE LIST OF ROUTES BETWEEN 2 FIRST NODES TO listRoutes
		if (curNode == dll_first(path)){
			curRouteList = getRoute(g, curNode->val.s, nextNode->val.s);
			dll_append(listRoutes, new_jval_v(curRouteList));
		}
		else{
			Dllist sameNode = new_dllist(), diffNode = new_dllist();
			
			curRouteList = getRoute(g, curNode->val.s, nextNode->val.s);
			dll_append(listRoutes, new_jval_v(curRouteList));
			//COMPARE prevRouteList and curRouteList and GET sameNode and diffNode
			compare(prevRouteList, curRouteList, sameNode, diffNode);
			
			//TRAVERSE listRoutes AND DELETE ALL ELEMENTS IN diffNODE FROM listRoutes SO THAT MIN ROUTES WILL BE TAKEN 
			if (!dll_empty(sameNode) && !dll_empty(diffNode)){
				dll_traverse(ptr1, listRoutes){
					ptr2 = (Dllist) jval_v(ptr1->val);
					if (countList(ptr2) == 1) continue;
					dll_traverse(ptr3, ptr2){
						dll_traverse(ptr4, diffNode){
							if (strcmp(ptr3->val.s, ptr4->val.s) == 0){
								ptr3 = dll_next(ptr3);
								dll_delete_node(dll_prev(ptr3));
								ptr3 = dll_prev(ptr3);
								break;
							}
						}
					}
				}
			}
			free_dllist(sameNode);
			free_dllist(diffNode);
		}
		prevRouteList = (Dllist) jval_v(dll_last(listRoutes)->val);
		curNode = curNode->flink;
	}

	return listRoutes;
}

//RETURN THE NUMBER OF SWITCH TIMES IN listRoutes
int countSwitch(Dllist listRoutes){
	int switch_times = 0;
	Dllist next;
	Dllist ptr = dll_first(listRoutes);
	Dllist cur;
	while(ptr->flink != listRoutes){
		cur = (Dllist) jval_v(ptr->val);
		next = (Dllist) jval_v(ptr->flink->val);
		if (!identicalList(cur, next)) switch_times++;
		ptr = ptr->flink;
	}
	return switch_times;
}

void printListRoutes(Dllist listRoutes){
	Dllist ptr1, ptr2, ptr3;
	dll_traverse(ptr1, listRoutes){
		ptr2 = (Dllist) jval_v(ptr1->val);
		printf("Catch : ");
		dll_traverse(ptr3, ptr2){
			printf("%s ",jval_s(ptr3->val));
		}
		printf("\n");
	}	
	printf("\n");
}

//CHECK IF THERE EXISTS A PATH BETWEEN start AND stop USING DFS
int existPath(Graph g, char *start, char *stop){
	if (!jrb_find_str(g.vertices, start) || !jrb_find_str(g.vertices, stop)) return 0;
	else{
		int i, n;
		char *station, output[countVertices][100];
		JRB node, visited = make_jrb();
		jrb_insert_str(visited, strdup(start), new_jval_i(1));
		Dllist ptr, queue = new_dllist();
		dll_append(queue, new_jval_s(strdup(start)));
		while(!dll_empty(queue)){
			ptr = dll_first(queue);
			station = strdup(ptr->val.s);
			dll_delete_node(ptr);
			n = outdegree(g, station, output);
			for (i = 0 ; i < n ; i++){
				if (strcmp(output[i], stop) == 0) return 1;
				if (!jrb_find_str(visited, output[i])){
					jrb_insert_str(visited, strdup(output[i]), new_jval_i(1));
					dll_append(queue, new_jval_s(strdup(output[i])));
				}
			}
		}
		return 0;
	}
	
}

//USING BFS, NOT EXACTLY POSSIBLE
void findPath(Graph g, char *start, char *stop, int numberofpaths){
	if(!jrb_find_str(g.vertices, start) || !jrb_find_str(g.vertices, stop)){
		printf("Station not found in either start or stop!!!\n");
		return;
	}
	if (strcmp(start, stop) == 0){
		printf("You are already there!!!");
		return;
	}
	if (hasEdge(g, start, stop)){
		Dllist ptr, route = getRoute(g, start, stop);
		printf("Route : \n");
		printf("Min switch times : 0\n");
		printf("Catch : ");
		dll_traverse(ptr, route){
			printf("%s ",ptr->val.s);
		}
		printf("\n\n");
		return;
	}
	
	int i, min, cur_min, outdeg, count = 0;
	char *last, output[countVertices][100];
	Dllist queue, initial_path, subpath, lastNode, newpath;
	
	//TO STORE findMinRoutes
	JRB listPath = make_jrb();
	
	//INITIALIZE A PATH WITH THE STARTING STATION
	initial_path = new_dllist();
	dll_append(initial_path, new_jval_s(strdup(start)));
	
	//USE A QUEUE TO KEEP TRACK OF PATHS
	queue = new_dllist();
	dll_append(queue, new_jval_v(initial_path));
	
	while(!dll_empty(queue)){
		subpath = (Dllist) jval_v(dll_first(queue)->val);		
		//CONSIDER THE LAST NODE OF EACH PATH IN QUEUE
		lastNode = dll_last(subpath);
		last = strdup(jval_s(lastNode->val));
		
		//IF THE LAST NODE IS stop, PRINT OUT THE PATH AND listRoutes
		if (strcmp(last, stop) == 0){
			if (count == numberofpaths) break;
			Dllist listRoutes = new_dllist();
			listRoutes = findMinRoutes(g, subpath);
			cur_min = countSwitch(listRoutes);
			if (jrb_empty(listPath)){
				min = cur_min;
				jrb_insert_int(listPath, min, new_jval_v(listRoutes));
				printf("Path : ");
				printPath(subpath);
				printf("Route : \n");
				printf("Min switch times : %d\n", min);
				printListRoutes(listRoutes);
				count++;
			}
			else if (min >= cur_min){
				min = cur_min;
				jrb_insert_int(listPath, min, new_jval_v(listRoutes));
				printf("Path : ");
				printPath(subpath);
				printf("Route : \n");
				printf("Min switch times : %d\n", min);
				printListRoutes(listRoutes);
				count++;
			}
	    	free_dllist(listRoutes);
		}
		//DEQUEUE
		dll_delete_node(dll_first(queue));
		
		//FIND STATIONS THAT THE LAST NODE POINTS TO
		outdeg = outdegree(g, last, output);
		
		for (i = 0 ; i < outdeg ; i++){
			//ONLY UPDATE UNVISITED NODE TO AVOID CYCLE
			if (isNotVisited(subpath, output[i])){
				
				newpath = copyList(subpath);
				//UPDATE NEWPATH FROM PREVIOUS PATH WITH A NEW lAST NODE
				dll_append(newpath, new_jval_s(strdup(output[i])));
		        //ENQUEUE NEWPATH TO QUEUE
				dll_append(queue, new_jval_v(newpath));	
			}
		}
	}
	jrb_free_tree(listPath);
	free_dllist(initial_path);
	free_dllist(queue);
}

void dropGraph(Graph g){
	JRB node, tree;
	jrb_traverse(node, g.edges){
		tree = (JRB) jval_v(node->val);
		jrb_free_tree(tree);
	}
	jrb_free_tree(g.edges);
	jrb_free_tree(g.vertices);
}
