
struct link_list_char_array {
	struct link_list_char_array *next;
	//struct link_list_char_array *prev;
	char data[512];
};

void ll_add(struct link_list_char_array *element, char *data, int len) {
	if (data == NULL || element == NULL)
		return;

	struct link_list_char_array *last = element;
	struct link_list_char_array *new_element;
	if (len == 0)
		len = 512;

	while (last->next != NULL) {
		last = last->next;
	}

	new_element = malloc(sizeof(struct link_list_char_array));
	if (new_element == NULL) {
		printf("Cannot allocate memory for data!\n");
		exit(1);
	}

	last->next = new_element;
	new_element->next = NULL;
	memcpy(new_element->data, data, len);
}

void ll_print(struct link_list_char_array *head) {
	if (head == NULL)
		return;

	struct link_list_char_array *curr = head->next;
	while (curr != NULL) {
		printf("%s\n", curr->data);
		curr = curr->next;
	}
}

int ll_count(struct link_list_char_array *head) {
	if (head == NULL)
		return -1;

	struct link_list_char_array *curr = head->next;
	int count = 0;
	while (curr != NULL) { curr = curr->next; count++; }
	return count;
}

int ll_get_index(int index, struct link_list_char_array *head, char *data) {
	if (head == NULL)
		return -1;

	struct link_list_char_array *curr = head->next;
	int count = 0;
	while (curr != NULL) { if (index == count) break; curr = curr->next; count++; }
	if (curr == NULL)
		return -1;

	if (index == count)
		sprintf(data, "%s", curr->data);

	return 0;
}

