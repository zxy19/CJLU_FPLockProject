export class Events<T> {
    private events: { [key: string]: ((data: T) => void)[] } = {};
    on(name: string, cb: (data: T) => void) {
        if (!this.events[name]) {
            this.events[name] = [];
        }
        this.events[name].push(cb);
    }
    emit(name: string, data: T) {
        if (!this.events[name]) {
            return;
        }
        this.events[name].forEach((cb) => cb(data))
    }
}