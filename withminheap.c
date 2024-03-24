#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define INF INT_MAX

typedef struct _node
{
    int total_dist;
    int wait_time;
    short row;
    short col;

    struct _node* parent;
}node;

void swap(node* elem1, node* elem2)
{
    node temp = *elem1;
    *elem1 = *elem2;
    *elem2 = temp;    
}

int right_child_idx(int idx, int heap_size)
{
    int right_idx = 2*idx + 2;

    if(right_idx < heap_size && right_idx >=0)
    {
        return right_idx;
    }
    else
    {
        return -1;
    }
}

int left_child_idx(int idx, int heap_size)
{
    int left_idx = 2*idx + 1;

    if(left_idx < heap_size && left_idx >=0)
    {
        return left_idx;
    }
    else
    {
        return -1;
    }
}

int parent_idx(int idx, int heap_size)
{
    if(idx > 0)
    {
        return ( (idx-1) / 2 );
    }
    else
    {
        return -1;
    }
}

void heapify(node* queue, int heap_size, int idx)
{
    int left_idx = left_child_idx(idx, heap_size);
    int right_idx = right_child_idx(idx, heap_size);

    int small = idx;

    if(left_idx != -1)
    {
        if(queue[left_idx].total_dist < queue[small].total_dist)
        {
            small = left_idx;
        }
    }

    if(right_idx != -1)
    {
        if(queue[right_idx].total_dist < queue[small].total_dist)
        {
            small = right_idx;
        }
    }

    if(small != idx)
    {
        swap(&queue[idx], &queue[small]);
        heapify(queue, heap_size, small);
    }
}

node pop(node* queue, int *heap_size)
{
    node val = queue[0];
    queue[0] = queue[*heap_size - 1];
    (*heap_size)--;
    heapify(queue, *heap_size, 0);
    return val;
}

void enque(node* queue, node val, int* heap_size)
{
    int idx = *heap_size;
    queue[idx] = val;

    int p_idx = parent_idx(idx, *heap_size);

    //UPWARD HEAPIFY
    if(p_idx != -1)
    {
        while(idx > 0 && (queue[p_idx].total_dist > queue[idx].total_dist))
        {
            swap(&queue[idx], &queue[p_idx]);
            idx = p_idx;
            p_idx = parent_idx(idx, *heap_size);
        }
    }
    (*heap_size)++;
}

void print_heap(node* queue, int heap_size)
{
    for(int i = 0; i < heap_size; i++)
    {
        printf("%d %d \n", queue[i].wait_time ,queue[i].total_dist);
    }
    printf("\n");
}

short** matrix_gen(FILE* file, short* row, short* col)
{

    fread(row, sizeof(short), 1, file);
    fread(col, sizeof(short), 1, file);

    short** arr = (short**)malloc(*row * sizeof(short*)); // ALLOCATING ARRAY OF POINTERS
    if(!arr)
    {
        return NULL;
    }

    //ALLOCATING EACH ROW OF MATRIX
    for (int i = 0; i < *row; i++)
    {
        arr[i] = (short*)malloc(*col * sizeof(short));
        if(!arr[i])
        {
            return NULL;
        }
    }

    //READING MATIX FROM BINARY FILE
    for(int i = 0; i < *row; i++)
    {
        for(int j = 0; j < *col; j++)
        {
            fread(&(arr[i][j]), sizeof(short), 1, file);
        }
    }

    return arr;
}

node** dijkstra(short ** matrix, short row, short col, node* queue, int* heap_size)
{

    node** all_nodes = (node**)malloc(sizeof(node*) * row);
    if(!all_nodes)
    {
        return NULL;
    }

    //ALLOCATING EACH ROW OF MATRIX
    for (int k = 0; k < row; k++)
    {
        all_nodes[k] = (node*)malloc(col * sizeof(node));
        if(!all_nodes[k])
        {
            return NULL;
        }
    }

    node temp;

    for(int i = 0; i < row; i++)
    {
        for(int m = 0; m < col; m++)
        {
            all_nodes[i][m].parent = NULL;
            all_nodes[i][m].total_dist = INF;
            all_nodes[i][m].row = i;
            all_nodes[i][m].col = m;
            all_nodes[i][m].wait_time = matrix[i][m];
        }
    }

    for(int j = 0; j < col; j++) // pushing last row
    {
        all_nodes[row-1][j].total_dist = 0;
        enque(queue, all_nodes[row-1][j], heap_size);
    }

    while(*heap_size > 0)
    {
        temp = pop(queue, heap_size);

        if(temp.col > 0) // has a left
        {
            if(temp.total_dist + temp.wait_time < all_nodes[temp.row][temp.col - 1].total_dist)
            {
                all_nodes[temp.row][temp.col - 1].parent = &all_nodes[temp.row][temp.col];
                all_nodes[temp.row][temp.col - 1].total_dist = temp.total_dist + temp.wait_time;
                enque(queue, all_nodes[temp.row][temp.col - 1], heap_size);
            }
        }        

        if(temp.col < col-1) // has a right
        {
           if(temp.total_dist + temp.wait_time < all_nodes[temp.row][temp.col + 1].total_dist)
            {
                all_nodes[temp.row][temp.col + 1].parent = &all_nodes[temp.row][temp.col];
                all_nodes[temp.row][temp.col + 1].total_dist = temp.total_dist + temp.wait_time;
                enque(queue, all_nodes[temp.row][temp.col + 1], heap_size);
            }
        }  
        if(temp.row > 0) // has a top
        {
            if(temp.total_dist + temp.wait_time < all_nodes[temp.row - 1][temp.col].total_dist)
            {
                all_nodes[temp.row - 1][temp.col].parent = &all_nodes[temp.row][temp.col];
                all_nodes[temp.row - 1][temp.col].total_dist = temp.total_dist + temp.wait_time;
                enque(queue, all_nodes[temp.row - 1][temp.col], heap_size);
            }
        } 
        if(temp.row < row-1) // has a bottom
        {
            if(temp.total_dist + temp.wait_time < all_nodes[temp.row + 1][temp.col].total_dist)
            {
                all_nodes[temp.row + 1][temp.col].parent = &all_nodes[temp.row][temp.col];
                all_nodes[temp.row + 1][temp.col].total_dist = temp.total_dist + temp.wait_time;
                enque(queue, all_nodes[temp.row + 1][temp.col], heap_size);
            }
        } 

    }

    for(int q = 0; q < col; q++)
    {
        all_nodes[0][q].total_dist += matrix[0][q];
    }
    return all_nodes;
}

int main(int argc, char *argv[])
{
    if(argc != 5)
    {
        return EXIT_FAILURE;
    }

    int counter = 0;

//GETTING MATRIX FROM BINARY FILE
    FILE* fptr1 = fopen(argv[1], "rb");
    if(fptr1 == NULL)
    {
        return EXIT_FAILURE;
    }

    short input_row;
    short input_col;

    short** matrix = matrix_gen(fptr1, &input_row, &input_col);
    if (matrix == NULL)
    {
        fclose(fptr1);
        return EXIT_FAILURE;
    }


// WRITING INPUT MATIX TO OUTPUT TEXT FILE
    FILE* fptr2 = fopen(argv[2], "w+");
    if(fptr2 == NULL)
    {
        for (int u = 0; u < input_row; u++)
        {
            free(matrix[u]);
        }
        free(matrix);
        fclose(fptr1);
        return EXIT_FAILURE;
    }

    fprintf(fptr2, "%hd %hd\n", input_row, input_col);

    for (int i = 0; i < input_row; i++)
    {
        for (int j = 0; j < input_col; j++)
        {
            fprintf(fptr2, "%hd", matrix[i][j]);
            if(j != input_col-1)
                fprintf(fptr2, " ");
        }
        fprintf(fptr2, "\n");
    }

    int num_nodes = input_col * input_row;
    int heap_size = 0;

    node* queue = (node*) malloc(sizeof(node)* num_nodes);
    if(queue == NULL)
    {
        for (int u = 0; u < input_row; u++)
        {
            free(matrix[u]);
        }
        free(matrix);
        fclose(fptr1);
        fclose(fptr2);
        return EXIT_FAILURE;
    }

    node** ans = dijkstra(matrix, input_row,input_col, queue, &heap_size);

    if(ans == NULL)
    {
        for (int u = 0; u < input_row; u++)
        {
            free(matrix[u]);
        }
        free(matrix);
        free(queue);
        fclose(fptr1);
        fclose(fptr2);
        return EXIT_FAILURE;
    }

// WRITING FASTEST TIME FILE
    FILE* fptr3 = fopen(argv[3], "w+");
    if(fptr3 == NULL)
    {
        for (int u = 0; u < input_row; u++)
        {
            free(matrix[u]);
        }
        free(matrix);
        fclose(fptr1);
        fclose(fptr2);
        free(queue);
        for (int z = 0; z < input_row; z++)
        {
            free(ans[z]);
        }
        free(ans);
        return EXIT_FAILURE;
    }

    fwrite(&input_col, sizeof(short), 1, fptr3);

    for(int w = 0; w < input_col; w++)
    {
        fwrite(&ans[0][w].total_dist, sizeof(int), 1, fptr3);
    }

// WRITING FASTEST PATH FILE
    FILE* fptr4 = fopen(argv[4], "w+");
    if(fptr4 == NULL)
    {
        for (int u = 0; u < input_row; u++)
        {
            free(matrix[u]);
        }
        free(matrix);
        fclose(fptr1);
        fclose(fptr2);
        fclose(fptr3);
        free(queue);
        for (int z = 0; z < input_row; z++)
        {
            free(ans[z]);
        }
        free(ans);
        return EXIT_FAILURE;
    }

    int small_idx = 0;
    for(int y = 0; y < input_col; y++)
    {
        if(ans[0][y].total_dist < ans[0][small_idx].total_dist)
        {
            small_idx = y;
        }
    }
    node* printer = &(ans[0][small_idx]);

    fwrite(&(printer->total_dist), sizeof(int), 1, fptr4);

    while(printer != NULL)
    {
        counter++;
        printer = printer->parent;
    }
    fwrite(&counter, sizeof(int), 1, fptr4);

    printer = &ans[0][small_idx];
    while(printer != NULL)
    {
        fwrite(&printer->row, sizeof(short), 1, fptr4);
        fwrite(&printer->col, sizeof(short), 1, fptr4);
        printer = printer->parent;
    }


// FREEING ALL ALLLOCATED MEMORY
    for (int u = 0; u < input_row; u++)
    {
        free(matrix[u]);
    }
    free(matrix);

    for (int z = 0; z < input_row; z++)
    {
        free(ans[z]);
    }
    free(ans);
    free(queue);
    fclose(fptr1);
    fclose(fptr2);
    fclose(fptr3);
    fclose(fptr4);
    return EXIT_SUCCESS;
}
