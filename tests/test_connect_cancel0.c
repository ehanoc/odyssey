
/*
 * machinarium.
 *
 * Cooperative multitasking engine.
*/

#include <machinarium.h>
#include <machinarium_test.h>

#include <arpa/inet.h>

static void
test_connect_fiber(void *arg)
{
	machine_io_t client = machine_create_io();
	test(client != NULL);

	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr("8.8.8.8");
	sa.sin_port = htons(1234);
	int rc;
	rc = machine_connect(client, (struct sockaddr *)&sa, INT_MAX);
	test(rc == -1);
	test(machine_cancelled());

	machine_free_io(client);
}

static void
test_waiter(void *arg)
{
	int id = machine_create_fiber(test_connect_fiber, NULL);
	test(id != -1);

	machine_sleep(0);

	int rc;
	rc = machine_cancel(id);
	test(rc == 0);

	rc = machine_wait(id);
	test(rc == 0);
}

void
test_connect_cancel0(void)
{
	machinarium_init();

	int id;
	id = machine_create(test_waiter, NULL);
	test(id != -1);

	int rc;
	rc = machine_join(id);
	test(rc != -1);

	machinarium_free();
}
