struct Order {
	1: string item,
	2: i16 amount,
	3: i32 price,
	4: i64 total
}

service Store {
	i32 getPrice(1: string item),
	bool order(1: string item, 2: i16 amount),
	list<Order> getReceipt()
}
