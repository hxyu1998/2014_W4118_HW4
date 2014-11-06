/* new GRR schedule */
#include "sched.h"
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/syscalls.h>
#include <trace/events/sched.h>

static void dequeue_task_grr(struct rq *rq, struct task_struct *p, int flags)
{
	struct sched_grr_entity *grr_se;
	struct grr_rq *grr_rq;

	grr_se = &p->grr;
	grr_rq = &rq->grr;

	raw_spin_lock(&grr_rq->grr_rq_lock);
	list_del_init(&grr_se->run_list);
	--grr_rq->grr_nr_running;
	raw_spin_unlock(&grr_rq->grr_rq_lock);
	dec_nr_running(rq);
}

static void enqueue_task_grr(struct rq *rq, struct task_struct *p, int flags)
{
	/* maybe this can simplified
	struct sched_grr_entity *grr_se = &p->grr;
	struct task_struct *p = container_of(grr_se, struct task_struct, rt);
	struct rq *rq = task_rq(p);
	struct grr_rq *grr_rq = rq->grr;
	struct list_head *queue = grr_rq->grr_rq_list;
	list_add(&grr_se->run_list, queue);
	grr_rq->nr_running++;
	*/

	struct sched_grr_entity *grr_se;
	struct grr_rq *grr_rq;
	struct list_head *queue;

	grr_se = &p->grr;
	grr_rq = &rq->grr;
	queue = &grr_rq->grr_rq_list;

	/*Ethan: I changed list_add to list_add_tail.*/
	raw_spin_lock(&grr_rq->grr_rq_lock);
	list_add_tail(&grr_se->run_list, queue);
	grr_rq->grr_nr_running++;
	raw_spin_unlock(&grr_rq->grr_rq_lock);
	inc_nr_running(rq);
}

static void requeue_task_grr(struct rq *rq, struct task_struct *p, int flags)
{
	struct sched_grr_entity *grr_se;
	struct grr_rq *grr_rq;
	struct list_head *head;

	grr_se = &p->grr;
	grr_rq = &rq->grr;
	head = &grr_rq->grr_rq_list;

	if (sizeof(grr_rq->grr_nr_running) == 1)
		return;

	raw_spin_lock(&grr_rq->grr_rq_lock);
	list_move_tail(&grr_se->run_list, head);
	raw_spin_unlock(&grr_rq->grr_rq_lock);
}

static void yield_task_grr(struct rq *rq)
{
	requeue_task_grr(rq, rq->curr , 0);
}

static struct task_struct *pick_next_task_grr(struct rq *rq)
{
	struct sched_grr_entity *grr_se;
	struct task_struct *p;
	struct grr_rq *grr_rq;
	struct list_head *queue;

	grr_rq = &rq->grr;

	if (!grr_rq->grr_nr_running)
		return NULL;

	queue = &grr_rq->grr_rq_list;
	grr_se = list_entry(queue->next, struct sched_grr_entity, run_list);
	return container_of(grr_se, struct task_struct, grr);
}

static void put_prev_task_grr(struct rq *rq, struct task_struct *prev)
{
}

static void task_tick_grr(struct rq *rq, struct task_struct *p, int queued)
{
	struct sched_grr_entity *grr_se;

	grr_se = &p->grr;
	if (--p->grr.time_slice)
		return;

	p->grr.time_slice = GRR_TIMESLICE;

	if (grr_se->run_list.prev != grr_se->run_list.next) {
		requeue_task_grr(rq, p, 0);
		set_tsk_need_resched(p);
		return;
	}
}

/*Ethan: this seems is used for group task schedule.*/
static void set_curr_task_grr(struct rq *rq)
{
	struct task_struct *p;

	p = rq->curr;
	p->se.exec_start = rq->clock_task;

}

static void check_preempt_curr_grr(struct rq *rq,
				struct task_struct *p, int flags)
{
}

/*Ethan: this seems is used for group task schedule.*/
static void switched_to_grr(struct rq *rq, struct task_struct *p)
{
	struct sched_grr_entity *grr_se;

	grr_se = &p->grr;
	grr_se->time_slice = GRR_TIMESLICE;
	INIT_LIST_HEAD(&grr_se->run_list);
}

static void prio_changed_grr(struct rq *rq, struct task_struct *p, int old)
{
	trace_printk("entering prio_changed_grr...\n");
	trace_printk("leaving prio_changed_grr...\n");
}

/*Ethan: Will be called by syscall scehd_rr_get_interval*/
static unsigned int get_rr_interval_grr(struct rq *rq, struct task_struct *t)
{
	if (t == NULL)
		return -EINVAL;
	return GRR_TIMESLICE;
}

void init_grr_rq(struct grr_rq *grr_rq)
{
	grr_rq->grr_nr_running = 0;
	INIT_LIST_HEAD(&grr_rq->grr_rq_list);
	raw_spin_lock_init(&grr_rq->grr_rq_lock);
}

/*====================For SMP====================*/
#ifdef CONFIG_SMP
static unsigned long __read_mostly max_load_balance_interval = HZ / 10;
static DEFINE_SPINLOCK(balancing);



static int load_balance_grr(int this_cpu, struct rq *this_rq,
			struct sched_domain *sd, enum cpu_idle_type idle,
			int *balance)
{
	struct grr_rq *grr_rq;
	int tempcpu = 0, temp = 0;
	int maxcpu = 0, mincpu = 0, max = 0, min = INT_MAX;
	struct rq *busiest;

	tempcpu = this_cpu;
	maxcpu = this_cpu;
	mincpu = this_cpu;
	cpumask_t eligibleCPUs;
	
	if (cpu_isset(this_cpu,fg_cpu_mask)) {
		eligibleCPUs = fg_cpu_mask;
	}
	else {
		eligibleCPUs = bg_cpu_mask;
	}
	char mask_str[1024];
	cpulist_scnprintf(mask_str, 1024, &eligibleCPUs);
	for_each_cpu(tempcpu, &eligibleCPUs) {
		
		busiest = cpu_rq(tempcpu);
		grr_rq = &busiest->grr;
		temp = grr_rq->grr_nr_running;

		if (max <= temp) {
			max = temp;
			maxcpu = tempcpu;
		}
		if (temp < min) {
			min = temp;
			mincpu = tempcpu;
		}
	}
	if (min != max-1 && min != max) {
		struct sched_grr_entity *seToMove, *dummy;
		struct rq *dst_rq;
		struct rq *src_rq;
		unsigned long flags;

		src_rq = cpu_rq(maxcpu);
		dst_rq = cpu_rq(mincpu);
		local_irq_save(flags);
		double_rq_lock(src_rq, dst_rq);
		list_for_each_entry_safe(seToMove, dummy,
				&src_rq->grr.grr_rq_list, run_list) {
			/* check if task can be moved
			 * do not move tasks that are running
			 *		OR
			 * cpus_allowed does not allow moving
			 * to least busy CPU
			 */
			struct task_struct *taskToMove;

			taskToMove = container_of(seToMove,
						struct task_struct, grr);
			if (!cpumask_test_cpu(mincpu, tsk_cpus_allowed(taskToMove)))
				continue;
			if (task_running(src_rq, taskToMove))
				continue;
			deactivate_task(src_rq, taskToMove, 0);
			set_task_cpu(taskToMove, mincpu);
			activate_task(dst_rq, taskToMove, 0);
			resched_task(dst_rq->curr);
			break;
		}
		double_rq_unlock(src_rq, dst_rq);
		local_irq_restore(flags);
	}
	return 1;
}

static void rebalance_domains_grr(int cpu, enum cpu_idle_type idle)
{
	int balance;
	struct rq *rq;
	unsigned long itvl;
	struct sched_domain *sd;
	unsigned long next_balance = jiffies + 60 * HZ;
	int update_next_balance, need_serialize;

	balance = 1;
	rq = cpu_rq(cpu);
	update_next_balance = 0;

	/*update_shares(cpu);*/

	rcu_read_lock();
	for_each_domain(cpu, sd) {
		if (!(sd->flags & SD_LOAD_BALANCE))
			continue;

		itvl = sd->balance_interval;
		if (idle != CPU_IDLE)
			itvl *= sd->busy_factor;

		itvl = msecs_to_jiffies(itvl);
		itvl = clamp(itvl, 1UL, max_load_balance_interval);

		need_serialize = sd->flags & SD_SERIALIZE;
		if (need_serialize) {
			if (!spin_trylock(&balancing))
				goto out;
		}

		if (time_after_eq(jiffies, sd->last_balance + itvl)) {
			if (load_balance_grr(cpu, rq, sd, idle, &balance))
				idle = CPU_NOT_IDLE;
			sd->last_balance = jiffies;
		}
		if (need_serialize)
			spin_unlock(&balancing);
out:
		if (time_after(next_balance, sd->last_balance + itvl)) {
			next_balance = sd->last_balance + itvl;
			update_next_balance = 1;
		}
		if (!balance)
			break;
	}
	rcu_read_unlock();

	if (likely(update_next_balance))
		rq->next_balance = next_balance;
}

static void run_rebalance_domains_grr(struct softirq_action *h)
{
	int this_cpu;
	struct rq *this_rq;
	enum cpu_idle_type idle;

	this_cpu = smp_processor_id();
	this_rq = cpu_rq(this_cpu);
	idle = this_rq->idle_balance ? CPU_IDLE : CPU_NOT_IDLE;
	rebalance_domains_grr(this_cpu, idle);
	/*nohz_idle_balance(this_cpu, idle);*/
}

static inline int on_null_domain(int cpu)
{
	return !rcu_dereference_sched(cpu_rq(cpu)->sd);
}

/*Ethan: looks like we just need to copy all the code from fair.c*/
void trigger_load_balance_grr(struct rq *rq, int cpu)
{
	if (time_after_eq(jiffies, rq->next_balance) &&
			likely(!on_null_domain(cpu)))
		raise_softirq(SCHED_GRR_SOFTIRQ);
/*#ifdef CONFIG_NO_HZ
	if (nohz_kick_needed(rq, cpu) && likely(!on_null_domain(cpu)))
		nohz_balancer_kick(cpu);
#endif*/
}

static void rq_online_grr(struct rq *rq)
{
}

static void rq_offline_grr(struct rq *rq)
{
}

static void switched_from_grr(struct rq *rq, struct task_struct *p)
{
}

static void pre_schedule_grr(struct rq *rq, struct task_struct *prev)
{
}

static void post_schedule_grr(struct rq *rq)
{
}

static void task_woken_grr(struct rq *rq, struct task_struct *p)
{
}

static int select_task_rq_grr(struct task_struct *p, int sd_flag, int flags)
{
	struct rq *rq;
	struct grr_rq *grr_rq;
	int cpu, idle_cpu;
	long loading, min_loading;

	min_loading = LONG_MAX;
	idle_cpu = -1;
	cpumask_t allowed_cpus;

	if (p->grr.group == 0)
		allowed_cpus = fg_cpu_mask;
	else
		allowed_cpus = bg_cpu_mask;

	for_each_cpu(cpu, &allowed_cpus) {

		rq = cpu_rq(cpu);
		grr_rq = &rq->grr;
		loading = grr_rq->grr_nr_running;

		if (loading < min_loading) {
			min_loading = loading;
			idle_cpu = cpu;
		}
	}
	if (idle_cpu == -1)
		return task_cpu(p);
	return idle_cpu;
}

#endif

const struct sched_class grr_sched_class = {
	.next			= &fair_sched_class,
	.dequeue_task		= dequeue_task_grr,
	.enqueue_task		= enqueue_task_grr,
	.yield_task		= yield_task_grr,
	.check_preempt_curr	= check_preempt_curr_grr,
	.pick_next_task		= pick_next_task_grr,
	.put_prev_task		= put_prev_task_grr,

#ifdef CONFIG_SMP
	.select_task_rq		= select_task_rq_grr,
	.rq_online		= rq_online_grr,
	.rq_offline		= rq_offline_grr,
	.task_woken		= task_woken_grr,
	.switched_from		= switched_from_grr,
	.pre_schedule		= pre_schedule_grr,
	.post_schedule		= post_schedule_grr,
#endif

	.set_curr_task		= set_curr_task_grr,
	.task_tick		= task_tick_grr,

	.get_rr_interval	= get_rr_interval_grr,

	.prio_changed		= prio_changed_grr,
	.switched_to		= switched_to_grr,
};

#ifdef CONFIG_SCHED_DEBUG
extern void print_grr_rq(struct seq_file *m, int cpu, struct grr_rq *grr_rq);

void print_grr_stats(struct seq_file *m, int cpu)
{
	struct rq *rq;
	struct grr_rq *grr_rq;

	rq = cpu_rq(cpu);
	rcu_read_lock();
	for (grr_rq = &rq->grr; grr_rq; grr_rq = NULL)
		print_grr_rq(m, cpu, grr_rq);
	rcu_read_unlock();
}
#endif

__init void init_sched_grr_class(void)
{
#ifdef CONFIG_SMP
	open_softirq(SCHED_GRR_SOFTIRQ, run_rebalance_domains_grr);
#endif
}
